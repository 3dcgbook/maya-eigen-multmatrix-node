#include <maya/MPxNode.h> 
#include <maya/MFnMatrixAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnPlugin.h>
#include <maya/MMatrix.h>
#include <maya/MGlobal.h>

class MayaMultMatrix : public MPxNode
{
public:
	MayaMultMatrix();
	virtual ~MayaMultMatrix() override;
	virtual MStatus     compute(const MPlug& plug, MDataBlock& data);
	static  void*       creator();
	static  MStatus     initialize();

public:
	static  MObject     input;
	static  MObject     output;
	static  MTypeId     id;

};


MTypeId     MayaMultMatrix::id(0x80003);
MObject     MayaMultMatrix::input;
MObject     MayaMultMatrix::output;


MayaMultMatrix::MayaMultMatrix()
{
}

MayaMultMatrix::~MayaMultMatrix()
{
}

MStatus MayaMultMatrix::initialize()
{
	
	MFnMatrixAttribute mAttr;
	input = mAttr.create("matrixIn", "i");
	mAttr.setArray(true);
	output = mAttr.create("matrixSum", "o");
	mAttr.setStorable(false);
	mAttr.setWritable(false);
	addAttribute(input);
	addAttribute(output);
	attributeAffects(input, output);

	return MS::kSuccess;
}


MStatus MayaMultMatrix::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus stat;
	if (plug != output) {
		return MS::kUnknownParameter;
	}
	MMatrix im;
	MArrayDataHandle arry_handle = data.inputArrayValue(input);
	for (unsigned i = 0; i < arry_handle.elementCount(); i++)
	{
		arry_handle.jumpToElement(i);
		MMatrix m = arry_handle.inputValue(&stat).asMatrix();
		im *= m;
	}
	data.outputValue(output).set(im);
	arry_handle.setAllClean();

	return MS::kSuccess;
}


void* MayaMultMatrix::creator()
{
	return new MayaMultMatrix();
}


MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, "3dcgbook.inc", "1.0", "Any");

	status = plugin.registerNode(
		"MayaMultMatrix",
		MayaMultMatrix::id, MayaMultMatrix::creator,
		MayaMultMatrix::initialize
	);
	return status;
}


MStatus uninitializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj);
	status = plugin.deregisterNode(MayaMultMatrix::id);
	return status;
}