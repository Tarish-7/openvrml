//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "VrmlNode.h"
#include "VrmlNamespace.h"
#include "VrmlNodeType.h"
#include "VrmlScene.h"
#include "MathUtils.h"
#include "VrmlBVolume.h"
#include "VrmlBSphere.h"
#include "VrmlRenderContext.h"

#include <stdio.h>		// sprintf

#ifndef NDEBUG
//#   define VRML_NODE_DEBUG
#endif

VrmlNodeType *VrmlNode::defineType(VrmlNodeType *t) { return t; }

VrmlNode::VrmlNode(VrmlScene *scene) :
  d_scene(scene),
  d_modified(false),
  d_routes(0),
  d_refCount(0),
  d_name(0)
{
  this->setBVolumeDirty(true);
}

VrmlNode::VrmlNode( const VrmlNode & ) :
  d_scene(0),
  d_modified(true),
  d_routes(0),
  d_refCount(0),
  d_name(0)
{
  this->setBVolumeDirty(true);
}


// Free name (if any) and route info.

VrmlNode::~VrmlNode() 
{
  // Remove the node's name (if any) from the map...
  if (d_name)
    {
      if (d_scene && d_scene->scope())
	d_scene->scope()->removeNodeName(this);
      delete [] d_name;
    }

  // Remove all routes from this node
  Route *r = d_routes;
  while (r)
    {
      Route *next = r->next();
      delete r;
      r = next;
    }

}


VrmlNode *VrmlNode::clone( VrmlNamespace *ns )
{
  if (isFlagSet())
    return ns->findNode(name());

  VrmlNode *n = this->cloneMe();
  if (n)
    {
      if (*name()) n->setName( name(), ns );
      setFlag();
      n->cloneChildren(ns);
    }
  return n;
}

void VrmlNode::cloneChildren( VrmlNamespace* ) {}


// Copy the routes to nodes in the given namespace.

void VrmlNode::copyRoutes( VrmlNamespace *ns ) const
{
  const char *fromName = name();
  VrmlNode *fromNode = fromName ? ns->findNode( fromName ) : 0;

  if ( fromNode )
    for (Route *r = d_routes; r; r = r->next() )
      {
	const char *toName = r->toNode()->name();
	VrmlNode *toNode = toName ? ns->findNode( toName ) : 0;
	if ( toNode )
	  fromNode->addRoute( r->fromEventOut(), toNode, r->toEventIn() );
      }
}

VrmlNode *VrmlNode::reference()
{
 ++d_refCount;
 return this; 
}

// Remove a reference to a node

void VrmlNode::dereference()
{
  if (--d_refCount == 0) delete this;
}


// Set the name of the node. Some one else (the parser) needs
// to tell the scene about the name for use in USE/ROUTEs.

void VrmlNode::setName(const char *nodeName, VrmlNamespace *ns)
{
  if (d_name) delete d_name;

  if (nodeName && *nodeName)
    {
      d_name = new char[strlen(nodeName)+1];
      strcpy(d_name, nodeName);
      if (ns) ns->addNodeName( this );
    }
  else
    d_name = 0;
}

// Retrieve the name of this node.

const char *VrmlNode::name() const
{
  return d_name ? d_name : "";
}

// Add to scene

void VrmlNode::addToScene( VrmlScene *scene, const char * /* relativeUrl */ )
{
  d_scene = scene;
}


// Node type tests

VrmlNodeAnchor*		VrmlNode::toAnchor() const { return 0; }
VrmlNodeAppearance*	VrmlNode::toAppearance() const { return 0; }
VrmlNodeAudioClip*	VrmlNode::toAudioClip() const { return 0; }
VrmlNodeBackground*	VrmlNode::toBackground() const { return 0; }
VrmlNodeBox*		VrmlNode::toBox() const { return 0; } //LarryD Mar 08/99
VrmlNodeChild*		VrmlNode::toChild() const { return 0; }
VrmlNodeColor*		VrmlNode::toColor() const { return 0; }
VrmlNodeCone*		VrmlNode::toCone() const { return 0; } //LarryD Mar 08/99
VrmlNodeCoordinate*	VrmlNode::toCoordinate() const { return 0; }
VrmlNodeCylinder* VrmlNode::toCylinder() const { return 0; } //LarryD Mar 08/99
VrmlNodeDirLight* VrmlNode::toDirLight() const { return 0; } //LarryD Mar 04/99
VrmlNodeElevationGrid* VrmlNode::toElevationGrid() const { return 0; } //LarryD Mar 09/99
VrmlNodeExtrusion*     VrmlNode::toExtrusion() const { return 0; } //LarryD Mar 09/99
VrmlNodeFog*		VrmlNode::toFog() const { return 0; }
VrmlNodeFontStyle*	VrmlNode::toFontStyle() const { return 0; }
VrmlNodeGeometry*	VrmlNode::toGeometry() const { return 0; }
VrmlNodeGroup*		VrmlNode::toGroup() const { return 0; }
VrmlNodeIFaceSet*	VrmlNode::toIFaceSet() const { return 0; }
VrmlNodeInline*		VrmlNode::toInline() const { return 0; }
VrmlNodeLight*		VrmlNode::toLight() const { return 0; }
VrmlNodeMaterial*	VrmlNode::toMaterial() const { return 0; }
VrmlNodeMovieTexture*	VrmlNode::toMovieTexture() const { return 0; }
VrmlNodeNavigationInfo*	VrmlNode::toNavigationInfo() const { return 0; }
VrmlNodeNormal*		VrmlNode::toNormal() const { return 0; }
VrmlNodePlaneSensor*	VrmlNode::toPlaneSensor() const { return 0; }
VrmlNodeSphereSensor*	VrmlNode::toSphereSensor() const { return 0; }
VrmlNodeCylinderSensor*	VrmlNode::toCylinderSensor() const { return 0; }
VrmlNodePointLight*	VrmlNode::toPointLight() const { return 0; }
VrmlNodeScript*		VrmlNode::toScript() const { return 0; }
VrmlNodeShape*		VrmlNode::toShape() const { return 0; }
VrmlNodeSound*		VrmlNode::toSound() const { return 0; }
VrmlNodeSphere* VrmlNode::toSphere() const { return 0; }      //LarryD Mar 08/99
VrmlNodeSpotLight*	VrmlNode::toSpotLight() const { return 0; }
VrmlNodeSwitch* VrmlNode::toSwitch() const { return 0; }      //LarryD Mar 08/99
VrmlNodeTexture*	VrmlNode::toTexture() const { return 0; }
VrmlNodeTextureCoordinate*	VrmlNode::toTextureCoordinate() const { return 0; }
VrmlNodeTextureTransform* VrmlNode::toTextureTransform() const { return 0; }
VrmlNodeTimeSensor*	VrmlNode::toTimeSensor() const { return 0; }
VrmlNodeTouchSensor*	VrmlNode::toTouchSensor() const { return 0; }
VrmlNodeTransform* VrmlNode::toTransform() const { return 0; } //LarryD Feb 24/99
VrmlNodeViewpoint*	VrmlNode::toViewpoint() const { return 0; }

VrmlNodeImageTexture* VrmlNode::toImageTexture() const { return 0; }
VrmlNodePixelTexture* VrmlNode::toPixelTexture() const { return 0; }

VrmlNodeLOD* VrmlNode::toLOD() const { return 0; }
VrmlNodeScalarInt* VrmlNode::toScalarInt() const { return 0; }
VrmlNodeOrientationInt* VrmlNode::toOrientationInt() const { return 0; }
VrmlNodePositionInt* VrmlNode::toPositionInt() const { return 0; }


VrmlNodeProto*		VrmlNode::toProto() const { return 0; }


// Routes

Route::Route( const char *fromEventOut, VrmlNode *toNode, const char *toEventIn ) :
  d_prev(0),
  d_next(0)
{
  d_fromEventOut = new char[strlen(fromEventOut)+1];
  strcpy(d_fromEventOut, fromEventOut);
  d_toNode = toNode;
  d_toEventIn = new char[strlen(toEventIn)+1];
  strcpy(d_toEventIn, toEventIn);
}

Route::Route( const Route &r )
{
  d_fromEventOut = new char[strlen(r.d_fromEventOut)+1];
  strcpy(d_fromEventOut, r.d_fromEventOut);
  d_toNode = r.d_toNode;
  d_toEventIn = new char[strlen(r.d_toEventIn)+1];
  strcpy(d_toEventIn, r.d_toEventIn);
}

Route::~Route()
{
  delete [] d_fromEventOut;
  delete [] d_toEventIn;
}

// Add a route from an eventOut of this node to an eventIn of another node.

void VrmlNode::addRoute(const char *fromEventOut,
			VrmlNode *toNode,
			const char *toEventIn)
{
#ifdef VRML_NODE_DEBUG
  fprintf(stderr,"%s::%s 0x%x addRoute %s\n",
	  nodeType()->getName(), name(), (unsigned) this, fromEventOut);
#endif


  // Check to make sure fromEventOut and toEventIn are valid names...
  
  // Is this route already here?
  Route *r;
  for (r=d_routes; r; r=r->next())
    {
      if (toNode == r->toNode() &&
	  strcmp(fromEventOut, r->fromEventOut()) == 0 &&
	  strcmp(toEventIn, r->toEventIn()) == 0 )
	return;       // Ignore duplicate routes
    }

  // Add route
  r = new Route(fromEventOut, toNode, toEventIn);
  if (d_routes)
    {
      r->setNext(d_routes);
      d_routes->setPrev(r);
    }
  d_routes = r;
}


// Remove a route from an eventOut of this node to an eventIn of another node.

void VrmlNode::deleteRoute(const char *fromEventOut,
			   VrmlNode *toNode,
			   const char *toEventIn)
{
  Route *r;
  for (r=d_routes; r; r=r->next())
    {
      if (toNode == r->toNode() &&
	  strcmp(fromEventOut, r->fromEventOut()) == 0 &&
	  strcmp(toEventIn, r->toEventIn()) == 0 )
	{
	  if (r->prev())
	    r->prev()->setNext(r->next());
	  if (r->next())
	    r->next()->setPrev(r->prev());
	  delete r;
	  break;
	}
    }
}

/**
 * @brief Get the routes from this node.
 *
 * @return a pointer to the first Route in the list.
 */
Route * VrmlNode::getRoutes() {
    return this->d_routes;
}

// Dirty bit - indicates node needs to be revisited for rendering.

void VrmlNode::setModified()
{
  d_modified = true;
  if (d_scene) d_scene->setModified(); 
}

bool VrmlNode::isModified() const
{
  return d_modified; 
}


void
VrmlNode::markPathModified(VrmlNodePath& path, bool mod, int flags) {
  VrmlNodePath::iterator i;
  VrmlNodePath::iterator end = path.end();
  if (flags & 0x001) {
    //cout << "VrmlNode[]::markPathModified():mod" << endl;
    for (i = path.begin(); i != end; ++i) {
      VrmlNode *c = *i;
      if (mod) {
	// do the proof that our invarient shows that this short
	// circuit is legal...
	//if (c->isModified()) 
	//break;
	c->setModified();
      } else
	c->clearModified();
    }
  }
  if (flags & 0x002) {
    //cout << "VrmlNode[]::markPathModified():bvol" << endl;
    for (i = path.begin(); i != end; ++i) {
      VrmlNode *c = *i;
      if (mod) {
	c->setBVolumeDirty(true);
      } else
	c->setBVolumeDirty(false);
    }
  }
}


void
VrmlNode::updateModified(VrmlNodePath& path, int flags)
{
  //cout << "VrmlNode[" << this << "]::updateModified()" << endl;
  if (this->d_modified||this->d_bvol_dirty) markPathModified(path, true, flags);
}


// note not virtual
//
void
VrmlNode::updateModified(int flags)
{
  VrmlNodePath path;
  updateModified(path, flags);
}


void VrmlNode::clearFlags()
{
  d_flag = false;
}

// Render

//bool VrmlNode::cull(Viewer *v, RenderContext* c)
//{
//if (c && c->getCullFlag()) {
//VrmlBVolume* bv = this->getBVolume();
//int r = v->isectViewVolume(*bv); // better not be null...
//if (r == VrmlBVolume::BV_OUTSIDE) {
//cout << "VrmlNode::render():OUTSIDE:culled" << endl;
//return true;
//} else if (r == VrmlBVolume::BV_INSIDE) {
//cout << "VrmlNode::render():INSIDE:no more cull tests" << endl;
//c->setCullFlag(false);
//return false;
//} else {
//cout << "VrmlNode::render():PARTIAL:continue cull tests" << endl;
//return false;
//}
//}
//return false;
//}


const VrmlBVolume*
VrmlNode::getBVolume() const
{
  //cout << "VrmlNode::getBVolume():inf" << endl;
  static VrmlBSphere* inf_bsphere = (VrmlBSphere*)0;
  if (!inf_bsphere) {
    inf_bsphere = new VrmlBSphere();
    inf_bsphere->setMAX();
  }
  ((VrmlNode*)this)->setBVolumeDirty(false);
  return inf_bsphere;
}


void
VrmlNode::setBVolume(const VrmlBVolume& v)
{
  cout << "VrmlNode::setBVolume():WARNING:not implemented" << endl;
}

void
VrmlNode::setBVolumeDirty(bool f)
{
  this->d_bvol_dirty = f;
  if (f && this->d_scene) // only if dirtying, not clearing
    this->d_scene->d_flags_need_updating = true;
}

bool
VrmlNode::isBVolumeDirty() const
{
  if (d_scene && d_scene->d_flags_need_updating) {
    d_scene->updateFlags();
    d_scene->d_flags_need_updating = false;
  }
  return this->d_bvol_dirty;
}

void VrmlNode::render(Viewer* v, VrmlRenderContext rc)
{
  //if (cull(v, c)) return;
  clearModified();
}

// Accumulate transformations for proper rendering of bindable nodes.

void VrmlNode::accumulateTransform(VrmlNode *)
{
  ;
}

VrmlNode* VrmlNode::getParentTransform() { return 0; }

void VrmlNode::inverseTransform(Viewer *v)
{
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(v);
}

void VrmlNode::inverseTransform(double m[4][4])
{
  //cout << "VrmlNode::inverseTransform" << endl;
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(m);
  else
    Midentity(m);
}


// Pass a named event to this node.

void VrmlNode::eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField *fieldValue)
{
#ifdef VRML_NODE_DEBUG
  cout << "eventIn "
       << nodeType()->getName()
       << "::"
       << (name() ? name() : "")
       << "."
       << eventName
       << " "
       << *fieldValue
       << endl;
#endif

  // Strip set_ prefix
  const char *origEventName = eventName;
  if ( strncmp(eventName, "set_", 4) == 0 )
    eventName += 4;

  // Handle exposedFields 
  if ( nodeType().hasExposedField( eventName ) )
    {
      setField(eventName, *fieldValue);
      char eventOutName[256];
      sprintf(eventOutName, "%s_changed", eventName);
      eventOut(timeStamp, eventOutName, *fieldValue);
      setModified();
    }

  // Handle set_field eventIn/field
  else if ( nodeType().hasEventIn( origEventName ) &&
	    nodeType().hasField( eventName ) )
    {
      setField(eventName, *fieldValue);
      setModified();
    }

  else
    cerr << "Error: unhandled eventIn " << nodeType().getName()
		<< "::" << name() << "." << origEventName << endl;

}


// Send a named event from this node.

void VrmlNode::eventOut(double timeStamp,
			const char *eventOut,
			const VrmlField &fieldValue)
{
#ifdef VRML_NODE_DEBUG
  fprintf(stderr,"%s::%s 0x%x eventOut %s\n",
	  nodeType()->getName(), name(),
	  (unsigned) this, eventOut);
#endif	  

  // Find routes from this eventOut
  Route *r;
  for (r=d_routes; r; r=r->next())
    {
      if (strcmp(eventOut, r->fromEventOut()) == 0)
	{
#ifdef VRML_NODE_DEBUG
	  cerr << "  => "
	       << r->toNode()->nodeType()->getName()
	       << "::"
	       << r->toNode()->name()
	       << "."
	       << r->toEventIn()
	       << endl;
#endif	  
	  VrmlField *eventValue = fieldValue.clone();
	  d_scene->queueEvent(timeStamp, eventValue,
			      r->toNode(), r->toEventIn());
	}
    }
}


ostream& operator<<(ostream& os, const VrmlNode& f)
{ return f.print(os, 0); }


ostream& VrmlNode::print(ostream& os, int indent) const
{
  const char *nm = name();
  for (int i=0; i<indent; ++i)
    os << ' ';

  if (nm && *nm)
    os << "DEF " << nm << " ";

  os << nodeType().getName() << " { ";

  // cast away const-ness for now...
  VrmlNode *n = (VrmlNode*)this;
  n->printFields(os, indent+INDENT_INCREMENT);

  os << " }";

  return os; 
}

// This should probably generate an error...
// Might be nice to make this non-virtual (each node would have
// to provide a getField(const char* name) method and specify
// default values in the addField(). The VrmlNodeType class would 
// have to make the fields list public.

ostream& VrmlNode::printFields(ostream& os, int /*indent*/)
{
  os << "# Error: " << nodeType().getName()
     << "::printFields unimplemented.\n";
  return os; 
}


ostream& VrmlNode::printField(ostream& os,
			      int indent,
			      const char *name,
			      const VrmlField& f)
{
  os << endl;
  for (int i=0; i<indent; ++i)
    os << ' ';
  os << name << ' ' << f;
  return os; 
}


// Set the value of one of the node fields. No fields exist at the
// top level, so reaching this indicates an error.

void VrmlNode::setField(const char *fieldName, const VrmlField &)
{
  theSystem->error("%s::setField: no such field (%s)",
		   nodeType().getName(), fieldName);
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNode::getField(const char *fieldName) const
{
  theSystem->error("%s::getField: no such field (%s)\n",
		   nodeType().getName(), fieldName);
  return 0;
}


// Retrieve a named eventOut/exposedField value.

const VrmlField *VrmlNode::getEventOut(const char *fieldName) const
{
  // Strip _changed prefix
  char shortName[256];
  int rootLen = strlen(fieldName) - strlen("_changed");
  if (rootLen >= (int) sizeof(shortName))
    rootLen = sizeof(shortName) - 1;
 
  if (rootLen > 0 && strcmp(fieldName+rootLen, "_changed") == 0)
    strncpy(shortName, fieldName, rootLen);
  else
    strncpy(shortName, fieldName, sizeof(shortName));

  // Handle exposedFields 
  if ( nodeType().hasExposedField( shortName ) )
    return getField( shortName );
  else if ( nodeType().hasEventOut( fieldName ) )
    return getField( fieldName );
  return 0;
}


//
//  VrmlNodeChild- should move to its own file
//
#include "VrmlNodeChild.h"

// Define the fields of all built in child nodes
VrmlNodeType *VrmlNodeChild::defineType(VrmlNodeType *t)
{
  return VrmlNode::defineType(t);
}

VrmlNodeChild::VrmlNodeChild(VrmlScene *scene) : VrmlNode(scene) {}

VrmlNodeChild* VrmlNodeChild::toChild() const
{
  return (VrmlNodeChild*)this; // discards const...
}


//
//  VrmlNodeTexture- should move to its own file
//
#include "VrmlNodeTexture.h"

VrmlNodeType *VrmlNodeTexture::defineType(VrmlNodeType *t)
{ return VrmlNode::defineType(t); }

VrmlNodeTexture::VrmlNodeTexture(VrmlScene *s) : VrmlNode(s) {}

VrmlNodeTexture::~VrmlNodeTexture() {}

VrmlNodeTexture* VrmlNodeTexture::toTexture() const
{ return (VrmlNodeTexture*) this; }


