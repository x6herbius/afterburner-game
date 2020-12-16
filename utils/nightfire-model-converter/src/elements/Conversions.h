#pragma once

#include "elements/HeaderV14.h"
#include "elements/HeaderV10Xash.h"
#include "elements/SequenceV14.h"
#include "elements/SequenceV10Xash.h"
#include "elements/VertexV14.h"
#include "elements/VertexV10Xash.h"
#include "elements/NormalV14.h"
#include "elements/NormalV10Xash.h"

namespace NFMDL
{
	void Convert(const HeaderV14& in, HeaderV10Xash& out);
	void Convert(const SequenceV14& in, SequenceV10Xash& out);
	void Convert(const VertexV14& in, VertexV10Xash& out);
	void Convert(const NormalV14& in, NormalV10Xash& out);
}
