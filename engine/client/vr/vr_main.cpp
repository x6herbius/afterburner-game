/*
vr_main.cpp -- OpenVR support
Copyright (C) 2018 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#ifdef XASH_OVR
#include "openvr.h"
#include "common.h"
extern "C"
{
#include "client.h"
#include "gl_local.h"
}

static vr::IVRSystem *vrsystem = NULL;
static vr::IVRCompositor *compositor = NULL;
static qboolean vr_initialized = false, vr_rendering = false;

static convar_t *vr_roomscale;

struct eye_t
{
	int fov_x, fov_y;
	int fbo;
	int texture;
	vr::HmdVector3_t origin;
	vr::HmdQuaternion_t orientation;
} eyes[2];

struct hand_t
{
	vr::HmdVector3_t vrorigin;
	vr::HmdQuaternion_t vrorientation;

	vec3_t origin;
	vec3_t angles;
};

const char *GetTrackedDeviceString( vr::TrackedDeviceIndex_t idx, vr::ETrackedDeviceProperty prop )
{
	static char buf[1024];
	vr::ETrackedPropertyError eError = vr::TrackedProp_Success;

	vrsystem->GetStringTrackedDeviceProperty( idx, prop, buf, sizeof( buf ), &eError );

	if( eError != vr::TrackedProp_Success )
	{
		Con_Reportf( "Can't get property %d, error %d, device %d", prop, eError, idx );
		buf[0] = 0;
		return buf;
	}

	return buf;
}

static void VR_Info_f( void )
{
	char buf[1024];

	if( !vr_initialized )
	{
		Con_Reportf( "OpenVR not initialized\n" );
		return;
	}

	Con_Reportf( "OpenVR Info:\n" );
	Con_Reportf( "Driver: %s\n", GetTrackedDeviceString( vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String ) );
	Con_Reportf( "Display: %s\n", GetTrackedDeviceString( vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String ) );
}

extern "C"
{

qboolean VR_Active( void )
{
	return vr_initialized /* check for hmd available */;
}

qboolean VR_IsRendering( void )
{
	return vr_rendering;
}

void VR_CalcRefDef( ref_viewpass_t *rvp )
{
	rvp->viewport[0] = 0;
	rvp->viewport[1] = 0;

	uint width, height;

	vrsystem->GetRecommendedRenderTargetSize( &width, &height );

	rvp->viewport[2] = width;
	rvp->viewport[3] = height;
}

void UpdateHMDMatrixPose( )
{
	vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	vr::VRCompositor( )->WaitGetPoses( m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0 );
}

void VR_UpdateScreen( void )
{
	if( cls.state != ca_active )
		return;

	vr::VREvent_t event;

	while( vrsystem->PollNextEvent( &event, sizeof( event ) ) )
	{
		Con_Reportf( "OpenVR Event: %s \n", vrsystem->GetEventTypeNameFromEnum( (vr::EVREventType) event.eventType ) );
	}

	if( !compositor->CanRenderScene( ) )
		return;

	vr_rendering = true;
	
	GL_BindFrameBuffer( eyes[0].fbo, eyes[0].texture );
	V_RenderView( );

	GL_BindFrameBuffer( eyes[1].fbo, eyes[1].texture );
	V_RenderView( );

	vr::Texture_t eyeTexture;
	eyeTexture.handle = ( void* ) eyes[0].texture;
	eyeTexture.eType = vr::TextureType_OpenGL;
	eyeTexture.eColorSpace = vr::ColorSpace_Gamma;
	auto eError = compositor->Submit( vr::Eye_Left, &eyeTexture );

	if( eError != vr::VRCompositorError_None )
	{
		Con_Reportf( S_ERROR "left eye submit reported error %d\n", eError );
	}
	
	eyeTexture.handle = ( void* ) eyes[1].texture;
	eError = compositor->Submit( vr::Eye_Right, &eyeTexture );

	if( eError != vr::VRCompositorError_None )
	{
		Con_Reportf( S_ERROR "right eye submit reported error %d\n", eError );
	}

	UpdateHMDMatrixPose( );

	vr_rendering = false;
	GL_BindFBO( 0 );
}

void VR_Init( void )
{
	vr::EVRInitError eError = vr::VRInitError_None;

	vr_roomscale = Cvar_Get( "vr_roomscale", "1", FCVAR_ARCHIVE, "set to 0 to play in seated position" );

	Cmd_AddCommand( "vr_info", VR_Info_f, "get vr info" );

	if( !Sys_CheckParm( "-vr" ) )
		return;

	// Loading the SteamVR Runtime
	vrsystem = vr::VR_Init( &eError, vr::VRApplication_Scene );

	if( eError != vr::VRInitError_None )
	{
		vrsystem = NULL;
		Con_Reportf( S_ERROR "Unable to init VR runtime: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription( eError ) );
		return;
	}

	Con_Reportf( "OpenVR initialized.\n" );
	vr_initialized = true;
}

void VR_RenderInit( void )
{
	if( !vr_initialized )
		return;

	if( !GL_Support( GL_FRAMEBUFFER_OBJECT ) ) // unlikely
	{
		Con_Reportf( S_ERROR "VR subsystem requires framebuffer object support!\n" );
		VR_Shutdown( );
		return;
	}
	
	if( !( compositor = vr::VRCompositor( ) ) ) // construct compositor
	{
		Con_Reportf( S_ERROR "Compositor failed to initialize!\n" );
		VR_Shutdown( );
		return;
	}

	int viewport[4];
	uint width, height;

	vrsystem->GetRecommendedRenderTargetSize( &width, &height );

	viewport[0] = viewport[1] = 0;
	viewport[2] = width;
	viewport[3] = height;

	for( int i = 0; i < 2; i++ )
	{
		float left, right, top, bottom;

		vrsystem->GetProjectionRaw( (vr::EVREye)i, &left, &right, &top, &bottom );

		eyes[i].texture = GL_CreateTexture( va( "*vreye%d", i ), width, height, NULL, (texFlags_t)(TF_CLAMP | TF_NEAREST) );
		eyes[i].fbo = R_AllocFrameBuffer( viewport );
		GL_BindFrameBuffer( eyes[i].fbo, eyes[i].texture );
		eyes[i].fov_x = RAD2DEG( atan( -left ) + atan( right ) );
		eyes[i].fov_y = RAD2DEG( atan( -top ) + atan( bottom ) );
		Con_Reportf( "Creating fbo with size %dx%d and FOV %fx%f\n", width, height, eyes[i].fov_x, eyes[i].fov_y );
	}

	GL_BindFBO( 0 );

	// vsync may cause syncing issues, better rely on OVR's mechanism
	// Cvar_FullSet( "gl_vsync", "0", FCVAR_READ_ONLY );

	compositor->CompositorBringToFront( );
	
	Con_Reportf( "OpenVR renderer initialized.\n" );
}

void VR_RenderShutdown( void )
{
	if( !vr_initialized )
		return;

	for( int i = 0; i < 2; i++ )
	{
		GL_FreeTexture( eyes[i].texture );
		R_FreeFrameBuffer( eyes[i].fbo );
	}

	Con_Reportf( "OpenVR renderer shutting down...\n" );
}

void VR_Shutdown( void )
{
	if( !vr_initialized )
		return;

	Con_Reportf( "OpenVR shutting down...\n" );

	vr::VR_Shutdown( );
	vr_initialized = false;
}

}
#endif // XASH_OVR