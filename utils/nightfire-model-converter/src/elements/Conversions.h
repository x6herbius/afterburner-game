#pragma once

#include "elements/HeaderV14.h"
#include "elements/HeaderV10Xash.h"
#include "elements/SequenceV14.h"
#include "elements/SequenceV10Xash.h"
#include "elements/BodyGroupV14.h"
#include "elements/BodyGroupV10Xash.h"

namespace NFMDL
{
	void Convert(const HeaderV14& in, HeaderV10Xash& out);
	void Convert(const SequenceV14& in, SequenceV10Xash& out);
	void Convert(const BodyGroupV14& in, BodyGroupV10Xash& out);
}
