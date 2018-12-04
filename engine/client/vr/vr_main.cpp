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

// a1ba: maybe it isn't cool to include all namespace
// but I don't care, it's really handy here
using namespace vr;

// NOTE: 
// Everything defined Xash's native types are in world coordinates
// In opposite, if OVR's types are used then it's HMD coordinates
struct vr_eye_t
{
	int fov_x, fov_y;
	int fbo;
	int texture;
	vec3_t origin;
	
	matrix4x4 modelViewMatrix;
};

struct vr_hand_t
{
	vec3_t origin;
	vec3_t angles;
};

struct vr_state_t
{
	qboolean active;
	int      currenteye;

	// TODO: count vortiguants with three hands and one big eye
	vr_eye_t  eye[2];
	vec3_t    headangles;
	vr_hand_t hand[2];

	uint glwidth, glheight;

	IVRSystem *system;
	IVRCompositor *compositor;

	TrackedDevicePose_t m_rTrackedDevicePose[k_unMaxTrackedDeviceCount];
};

static vr_state_t vrs;
static convar_t *vr_roomscale;
static const vec3_t HL_TO_VR = { 2.3f / 10.f, 2.2f / 10.f, 2.3f / 10.f };
static const vec3_t VR_TO_HL = { 1.f / HL_TO_VR[0], 1.f / HL_TO_VR[1], 1.f / HL_TO_VR[2] };


static const char *GetTrackedDeviceString( ETrackedDeviceProperty prop, TrackedDeviceIndex_t idx = k_unTrackedDeviceIndex_Hmd )
{
	static char buf[1024];
	ETrackedPropertyError eError = TrackedProp_Success;

	vrs.system->GetStringTrackedDeviceProperty( idx, prop, buf, sizeof( buf ), &eError );

	if( eError != TrackedProp_Success )
	{
		Con_Reportf( "Can't get property %d, error %d, device %d", prop, eError, idx );
		buf[0] = 0;
	}

	return buf;
}

static void VR_Info_f( void )
{
	char buf[1024];

	if( !vrs.active )
	{
		Con_Reportf( "OpenVR not active\n" );
		return;
	}

	Con_Reportf( "OpenVR Info:\n" );
	Con_Reportf( "Driver: %s\n", GetTrackedDeviceString( vr::Prop_TrackingSystemName_String ) );
	Con_Reportf( "Display: %s\n", GetTrackedDeviceString( vr::Prop_SerialNumber_String ) );
	Con_Reportf( "Resolution: %udx%ud\n", vrs.glwidth, vrs.glheight );
}

static void Matrix4x4_FromOVR( matrix4x4 out, const HmdMatrix34_t &mat )
{
	for( int i = 0; i < 4; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			out[i][j] = mat.m[j][i];
		}
	}

	out[0][3] = 0.0f;
	out[1][3] = 0.0f;
	out[2][3] = 0.0f;
	out[3][3] = 1.0f;
}

static void Matrix4x4_FromOVR( matrix4x4 out, const HmdMatrix44_t &mat )
{
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 4; j++ )
		{
			out[i][j] = mat.m[j][i];
		}
	}
}

void VectorToQuakeCoordinates( vec3_t v )
{
	vec3_t temp;
	temp[0] = -v[2];
	temp[1] = -v[0];
	temp[2] = v[1];
	VectorCopy( temp, v );
}

void Matrix3x4_ToQuaternion( const matrix3x4 in, vec4_t out )
{
	out[0] = sqrt( max( 0, 1 + in[0][0] - in[1][1] - in[2][2] ) ) / 2;
	out[1] = sqrt( max( 0, 1 - in[0][0] + in[1][1] - in[2][2] ) ) / 2;
	out[2] = sqrt( max( 0, 1 - in[0][0] - in[1][1] + in[2][2] ) ) / 2;
	out[0] = copysign( out[0], in[2][1] - in[1][2] );
	out[1] = copysign( out[1], in[0][2] - in[2][0] );
	out[2] = copysign( out[2], in[1][0] - in[0][1] );
	out[3] = sqrt( max( 0, 1 + in[0][0] + in[1][1] + in[2][2] ) ) / 2;
}


void Matrix3x4_ToQuaternion( const HmdMatrix34_t in, vec4_t out )
{
	Matrix3x4_ToQuaternion( in.m, out );
	VectorToQuakeCoordinates( out );
}

void VectorRotateQuaternion( const vec3_t v, const vec4_t q, vec3_t result )
{
	vec3_t u;
	float s = q[3];

	VectorCopy( q, u );

	float uvDot = DotProduct( u, v );
	float uuDot = DotProduct( u, u );
	
	vec3_t uvCross;
	CrossProduct( u, v, uvCross );

	VectorMAMAM( 2.0f * uvDot, u, s*s - uuDot, v, 2.0f * s, uvCross, result );
}

#if 0
void VectorToQuakeAngles( vec3_t v )
{
	vec3_t temp;
	temp[YAW] = -v[2];
	temp[PITCH] = -v[0];
	temp[ROLL] = v[1];
	VectorCopy( temp, v );
}
#endif

static void UpdateHMDMatrixPose( )
{
	vrs.compositor->WaitGetPoses( vrs.m_rTrackedDevicePose, ARRAYSIZE( vrs.m_rTrackedDevicePose ), NULL, 0 );

	for( int i = 0; i < k_unMaxTrackedDeviceCount; i++ )
	{
		TrackedDevicePose_t *dev = &vrs.m_rTrackedDevicePose[i];

		if( !dev->bDeviceIsConnected )
			continue;

		if( !dev->bPoseIsValid )
			continue;

		if( dev->eTrackingResult != TrackingResult_Running_OK )
			continue;

		// I suppose we have only one HMD and only two controllers
		switch( vrs.system->GetTrackedDeviceClass( i ) )
		{
		case TrackedDeviceClass_HMD:
		{
			vec3_t headorigin, eyestohead[2], eyesorigin[2];
			vec4_t headq, playerorigin;
			cl_entity_t *e = CL_GetLocalPlayer( );

			Matrix3x4_OriginFromMatrix( dev->mDeviceToAbsoluteTracking.m, headorigin );
			Matrix3x4_OriginFromMatrix( vrs.system->GetEyeToHeadTransform( Eye_Left ).m, eyestohead[0] );
			Matrix3x4_OriginFromMatrix( vrs.system->GetEyeToHeadTransform( Eye_Right ).m, eyestohead[1] );
			Matrix3x4_ToQuaternion( dev->mDeviceToAbsoluteTracking, headq );

			QuaternionAngle( headq, vrs.headangles );

			VectorRotateQuaternion( eyestohead[0], headq, eyesorigin[0] );
			VectorRotateQuaternion( eyestohead[1], headq, eyesorigin[1] );

			VectorToQuakeCoordinates( headorigin );
			VectorToQuakeCoordinates( eyesorigin[0] );
			VectorToQuakeCoordinates( eyesorigin[1] );

			VectorCopy( e->origin, playerorigin );

			for( int i = 0; i < 3; i++ )
				headorigin[i] = playerorigin[i] + HL_TO_VR[i] * headorigin[i];

			VectorAdd( eyesorigin[0], headorigin, vrs.eye[0].origin );
			VectorAdd( eyesorigin[1], headorigin, vrs.eye[1].origin );
			break;
		}
		case TrackedDeviceClass_Controller:
		{
			// TODO: implement tracking controllers
			// TODO: provide a way to send contoller params to game dll
			break;
		}
		default: break;
		}
	}
}

extern "C"
{
qboolean VR_Active( void )
{
	return vrs.active && VR_IsHmdPresent();
}

int VR_IsRendering( void )
{
	return vrs.currenteye + 1;
}

void VR_CalcRefDef( ref_viewpass_t *rvp )
{
	rvp->viewport[0] = 0;
	rvp->viewport[1] = 0;

	rvp->viewport[2] = vrs.glwidth;
	rvp->viewport[3] = vrs.glheight;

	VectorCopy( vrs.eye[vrs.currenteye].origin, rvp->vieworigin );
	VectorCopy( vrs.headangles, rvp->viewangles );

	// TODO: replace with matrices?
	rvp->fov_x = 110;
	rvp->fov_y = 110;
}

void VR_SetupProjectionMatrix( matrix4x4 m )
{
	RI.farClip = R_GetFarClip( );
	float zNear = 0.1f;
	float zFar = max( 256.0, RI.farClip );

	HmdMatrix44_t hmd = vrs.system->GetProjectionMatrix( ( EVREye ) vrs.currenteye, zNear, zFar );

	Matrix4x4_FromOVR( m, hmd );
}

void VR_UpdateScreen( void )
{
	if( cls.state != ca_active )
		return;

	VREvent_t event;

	while( vrs.system->PollNextEvent( &event, sizeof( event ) ) )
	{
		Con_Reportf( "OpenVR Event: %s \n", vrs.system->GetEventTypeNameFromEnum( (EVREventType) event.eventType ) );
	}

	if( !vrs.compositor->CanRenderScene( ) )
		return;

	for( vrs.currenteye = 0; vrs.currenteye < 2; vrs.currenteye++ )
	{
		// actually only binds FBO, texture is already bound during RenderInit
		GL_BindFrameBuffer( vrs.eye[vrs.currenteye].fbo, vrs.eye[vrs.currenteye].texture );
		V_RenderView( );

		Texture_t eyeTexture;
		eyeTexture.handle = ( void* ) vrs.eye[vrs.currenteye].texture;
		eyeTexture.eType = TextureType_OpenGL;
		eyeTexture.eColorSpace = ColorSpace_Gamma;

		vr::EVRCompositorError eError = vrs.compositor->Submit( (EVREye) vrs.currenteye, &eyeTexture );

		if( eError != vr::VRCompositorError_None )
		{
			Con_Reportf( S_ERROR "Eye submit %d reported error %d\n", vrs.currenteye, eError );
		}
	}

	vrs.currenteye = -1;
	GL_BindFBO( 0 );

	UpdateHMDMatrixPose( );
}

void ::VR_Init( void )
{
	EVRInitError eError = VRInitError_None;

	memset( &vrs, 0, sizeof( vrs ) );

	vr_roomscale = Cvar_Get( "vr_roomscale", "1", FCVAR_ARCHIVE, "set to 0 to play in seated position" );
	Cmd_AddCommand( "vr_info", VR_Info_f, "get vr info" );

	if( !Sys_CheckParm( "-vr" ) )
		return;

	vrs.system = VR_Init( &eError, vr::VRApplication_Scene );

	if( eError != vr::VRInitError_None )
	{
		vrs.system = NULL;
		Con_Reportf( S_ERROR "Unable to init VR runtime: %s\n", VR_GetVRInitErrorAsEnglishDescription( eError ) );
		return;
	}

	Con_Reportf( "OpenVR active.\n" );
	vrs.active = true;
}

void VR_RenderInit( void )
{
	if( !vrs.active )
		return;

	if( !GL_Support( GL_FRAMEBUFFER_OBJECT ) ) // unlikely
	{
		Con_Reportf( S_ERROR "VR subsystem requires framebuffer object support!\n" );
		::VR_Shutdown( );
		return;
	}
	
	if( !( vrs.compositor = vr::VRCompositor( ) ) ) // construct compositor
	{
		Con_Reportf( S_ERROR "Compositor failed to initialize!\n" );
		::VR_Shutdown( );
		return;
	}

	vrs.system->GetRecommendedRenderTargetSize( &vrs.glwidth, &vrs.glheight );

	for( int i = 0; i < 2; i++ )
	{
		float left, right, top, bottom;

		vrs.system->GetProjectionRaw( (vr::EVREye)i, &left, &right, &top, &bottom );

		vrs.eye[i].texture = GL_CreateTexture( va( "*vreye%d", i ), vrs.glwidth, vrs.glheight, NULL, (texFlags_t)(TF_CLAMP | TF_NEAREST) );
		vrs.eye[i].fbo = R_AllocFrameBuffer( vrs.glwidth, vrs.glheight );
		GL_BindFrameBuffer( vrs.eye[i].fbo, vrs.eye[i].texture ); // bind texture to framebuffer here, so it would be cached
		vrs.eye[i].fov_x = RAD2DEG( atan( -left ) + atan( right ) );
		vrs.eye[i].fov_y = RAD2DEG( atan( -top ) + atan( bottom ) );
		Con_Reportf( "Creating fbo with size %dx%d and FOV %fx%f\n", vrs.glwidth, vrs.glheight, vrs.eye[i].fov_x, vrs.eye[i].fov_y );
	}

	GL_BindFBO( 0 ); // reset FBO

	// vsync may cause syncing issues, better rely on OVR's mechanism
	Cvar_FullSet( "gl_vsync", "0", FCVAR_READ_ONLY );

	vrs.compositor->CompositorBringToFront( );
	
	Con_Reportf( "OpenVR renderer initialized.\n" );
}

void VR_RenderShutdown( void )
{
	if( !vrs.active )
		return;

	for( int i = 0; i < 2; i++ )
	{
		GL_FreeTexture( vrs.eye[i].texture );
		R_FreeFrameBuffer( vrs.eye[i].fbo );
	}

	Con_Reportf( "OpenVR renderer shutting down...\n" );
}

void ::VR_Shutdown( void )
{
	if( !VR_Active() )
		return;

	Con_Reportf( "OpenVR shutting down...\n" );

	vr::VR_Shutdown( );
	vrs.active = false;
}

}
#endif // XASH_OVR