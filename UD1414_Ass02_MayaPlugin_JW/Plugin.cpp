#include "Plugin.h"


MCallbackId _CBid;
MCallbackIdArray _CBidArray;


void f_nameChange(MObject& node, const MString& str, void* clientData)
{
	if (node.hasFn(MFn::kMesh))
	{
		MFnMesh mesh(node);
		MGlobal::displayInfo("Mesh name changed to: " + (MString)mesh.name());
		//MMessage::removeCallback(MMessage::currentCallbackId());
	}
	else if (node.hasFn(MFn::kTransform) == true)
	{
		bool hasShapes = false;
		MFnTransform trans(node);
		for (int i = 0; i < trans.childCount(); i++)
		{
			MObject child = trans.child(i);
			if (child.hasFn(MFn::kMesh))
			{
				hasShapes = true;
			}
		}
		if(hasShapes)
			MGlobal::displayInfo("Transform name changed to: " + (MString)trans.name());
	}
}

void f_transformChanged(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData)
{
	if(msg & MNodeMessage::AttributeMessage::kAttributeSet)
	//if (plug_1.node().hasFn(MFn::kTransform))
	{
		MStatus result;
		MFnTransform transform(plug_1.node());
		//MString att(plug_1.node());
		MGlobal::displayInfo(MString(plug_1.info().asChar()));
		std::string attName(plug_1.info().asChar());
		if (attName.find("translate") != std::string::npos)
		{
			MVector trans = transform.getTranslation(MSpace::kPostTransform, &result);
			MGlobal::displayInfo("NODE: " + transform.fullPathName() + " Translation changed: (" + MString() + trans.x + " , " + MString() + trans.y + " , " + MString() + trans.z + ")");
		}
		else if (attName.find("rotate") != std::string::npos)
		{
			MEulerRotation rotation;
			transform.getRotation(rotation);
			MGlobal::displayInfo("NODE: " + transform.fullPathName() + " Scaling changed: (" + MString() + rotation.x + " , " + MString() + rotation.y +" , " + MString() + rotation.z + ")");
			
		}
		else if (attName.find("scale") != std::string::npos)
		{
			double scale[3];
			transform.getScale(scale);
			MGlobal::displayInfo("NODE: " + transform.fullPathName() + "Scale changed: (" + MString() + scale[0] + " , " + MString() + scale[1] + " , " + MString() + scale[2] + ")");
		}
	}
}

void f_meshAttributeChanged(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData)
{
	// Standard string for use with find() function
	std::string plugName(plug_1.name().asChar());


	if (plugName.find("doubleSided") != std::string::npos && MNodeMessage::AttributeMessage::kAttributeSet)
	{
		//MGlobal::displayInfo(MString(plug_1.name().asChar()));
		MStatus result;
		MFnMesh mNode(plug_1.node(), &result);

		
		if (result)
		{
		
			MGlobal::displayInfo("\nNew Mesh node path: " + mNode.fullPathName() + "\n");
			//DO STUFF
			MPointArray vertices;
			MFloatVectorArray normals;
			MFloatArray uArray;
			MFloatArray vArray;
			mNode.getPoints(vertices, MSpace::kPostTransform);
			mNode.getNormals(normals, MSpace::kPostTransform);
			mNode.getUVs(uArray, vArray);
			//if (mNode.getPoints(vertices, MSpace::kWorld) == MS::kSuccess)
			//{
				MGlobal::displayInfo("Number of vertices: " + MString() + vertices.length());
				for (int i = 0; i < vertices.length(); i++)
				{
					MGlobal::displayInfo(MString() + (int)(i + 1) + ".");
					MGlobal::displayInfo("v: " +MString() + vertices[i].x + ", " + MString() + vertices[i].y + ", " + MString() + vertices[i].z);
					MGlobal::displayInfo("n: " + MString() + normals[i].x + ", " + MString() + normals[i].y + ", " + MString() + normals[i].z);
					MGlobal::displayInfo("uv: " + MString() + uArray[i] + ", " + MString() + vArray[i]);
				}
				MGlobal::displayInfo("\n\n");

			//}
		}
		
	}
	
	MObject ob(plug_1);
	if (plug_1. == MFn::kPolyMoveVertex)
	{
		MGlobal::displayInfo("MOVED VERT");
	}
	//MGlobal::displayInfo(plug_1.asMObject().apiTypeStr());

}

void f_polyChanged(MObject& node, void* clientData)
{

	MGlobal::displayInfo("MESH CHANGED, BITCH");

}

void f_testDirtyCallback(MObject& node, void* clientData)
{
	MGlobal::displayInfo("DIIIIIIIIIIIIIIIRTY SHIET, BRO");
}

void f_testPolyComponentIdCallback(MUintArray componentIds[], unsigned int count, void *clientData)
{
	MGlobal::displayInfo("CHANNNNNNNNNNNGEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE SHIET, BRO");
}

void f_meshCreated(MObject& node, void* clientData)
{
	if (node.hasFn(MFn::kMesh))
	{
		_CBidArray.append(MNodeMessage::addAttributeChangedCallback(node, f_meshAttributeChanged));
		_CBidArray.append(MPolyMessage::addPolyTopologyChangedCallback(node, f_polyChanged));
		//_CBidArray.append(MNodeMessage::addNodeDirtyCallback(node, f_testDirtyCallback));
		//_CBidArray.append(MPolyMessage::addPolyComponentIdChangedCallback(node, wantIDchanges, 3, f_testPolyComponentIdCallback));

		
	}
	if (node.hasFn(MFn::kTransform))
	{
		_CBidArray.append(MNodeMessage::addAttributeChangedCallback(node, f_transformChanged));
	}


}

EXPORT MStatus initializePlugin(MObject obj)
{
	//Error Checking Variable
	MStatus result = MS::kSuccess;

	MFnPlugin editorPlugin(obj, "Level Editor", "1.0", "Any", &result);
	if (MFAIL(result))
	{
		CHECK_MSTATUS(result);
	}

	MGlobal::displayInfo("Level Editor plugin loaded.");
	//MObject objecto = obj;
	_CBidArray.append(MNodeMessage::addNameChangedCallback(MObject::kNullObj, f_nameChange));
	_CBidArray.append(MDGMessage::addNodeAddedCallback(f_meshCreated));
	/*MStatus lel;
	_CBidArray.append(MNodeMessage::addAttributeChangedCallback(MObject::kNullObj, f_transformChanged, NULL, &lel));
	if (lel == MS::kSuccess)
	{
		MGlobal::displayInfo("Could be added bitch");
	}*/
	//_CBidArray.

	return result;
	
}



EXPORT MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);
	
	MGlobal::displayInfo("Level Editor plugin unloaded.");

	MMessage::removeCallbacks(_CBidArray);

	return MS::kSuccess;
}