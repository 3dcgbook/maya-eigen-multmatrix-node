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
#include <Eigen/Dense>


class EigenMultMatrix : public MPxNode
{
public:
	EigenMultMatrix();
	virtual ~EigenMultMatrix();
	virtual MStatus     compute(const MPlug& plug, MDataBlock& data);
	static  void*       creator();
	static  MStatus     initialize();

public:
	static  MObject     input;
	static  MObject     output;
	static  MTypeId     id;
};


MTypeId     EigenMultMatrix::id(0x80001);
MObject     EigenMultMatrix::input;
MObject     EigenMultMatrix::output;


EigenMultMatrix::EigenMultMatrix()
{
}


EigenMultMatrix::~EigenMultMatrix()
{
}


MStatus EigenMultMatrix::initialize()
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


MStatus EigenMultMatrix::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus stat;
	if (plug != output) {
		return MS::kUnknownParameter;
	}
	Eigen::Matrix4d im = Eigen::Matrix4d::Identity(4, 4);
	MArrayDataHandle arry_handle = data.inputArrayValue(input);
	for (unsigned i = 0; i < arry_handle.elementCount(); i++)
	{
		arry_handle.jumpToElement(i);
		MMatrix m = arry_handle.inputValue(&stat).asMatrix();
		Eigen::Matrix4d em = reinterpret_cast<Eigen::Matrix4d&>(m);
		im *= em;
	}
	MMatrix out_matrix = reinterpret_cast<MMatrix&>(im);
	data.outputValue(output).set(out_matrix);
	arry_handle.setAllClean();

	return MS::kSuccess;
}


void* EigenMultMatrix::creator()
{
	return new EigenMultMatrix();
}


MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, "3dcgbook.inc", "1.0", "Any");

	status = plugin.registerNode(
		"EigenMultMatrix",
		EigenMultMatrix::id, EigenMultMatrix::creator,
		EigenMultMatrix::initialize
	);
	return status;
}


MStatus uninitializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj);
	status = plugin.deregisterNode(EigenMultMatrix::id);
	return status;
}