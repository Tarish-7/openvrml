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

#include "VrmlNodeIFaceSet.h"
#include "VrmlNodeType.h"
#include "VrmlNodeColor.h"
#include "VrmlNodeCoordinate.h"
#include "VrmlNodeNormal.h"
#include "VrmlNodeTextureCoordinate.h"
#include "Viewer.h"

static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeIFaceSet(s); }

// Define the built in VrmlNodeType:: "IndexedFaceSet" fields

VrmlNodeType *VrmlNodeIFaceSet::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("IndexedFaceSet", creator);
      t->reference();
    }

  VrmlNodeIndexedSet::defineType(t);	// Parent class

  t->addEventIn("set_normalIndex", VrmlField::MFINT32);
  t->addEventIn("set_texCoordIndex", VrmlField::MFINT32);
  t->addExposedField("normal", VrmlField::SFNODE);
  t->addExposedField("texCoord", VrmlField::SFNODE);
  t->addField("ccw", VrmlField::SFBOOL);
  t->addField("convex", VrmlField::SFBOOL);
  t->addField("creaseAngle", VrmlField::SFFLOAT);
  t->addField("normalIndex", VrmlField::MFINT32);
  t->addField("normalPerVertex", VrmlField::SFBOOL);
  t->addField("solid", VrmlField::SFBOOL);
  t->addField("texCoordIndex", VrmlField::MFINT32);

  return t;
}


VrmlNodeType & VrmlNodeIFaceSet::nodeType() const
{
    return *defineType(0);
}


VrmlNodeIFaceSet::VrmlNodeIFaceSet(VrmlScene *scene) :
  VrmlNodeIndexedSet(scene),
  d_ccw(true),
  d_convex(true),
  d_creaseAngle(0.0),
  d_normalPerVertex(true),
  d_solid(true)
{
  this->setBVolumeDirty(true);
}

VrmlNodeIFaceSet::~VrmlNodeIFaceSet()
{
}


VrmlNode *VrmlNodeIFaceSet::cloneMe() const
{
  return new VrmlNodeIFaceSet(*this);
}

void VrmlNodeIFaceSet::cloneChildren(VrmlNamespace* ns)
{
  if (d_color.get())
    d_color.set(d_color.get()->clone(ns));
  if (d_coord.get())
    d_coord.set(d_coord.get()->clone(ns));
  if (d_normal.get())
    d_normal.set(d_normal.get()->clone(ns));
  if (d_texCoord.get())
    d_texCoord.set(d_texCoord.get()->clone(ns));
}


bool VrmlNodeIFaceSet::isModified() const
{
  return ( d_modified ||
	   (d_color.get() && d_color.get()->isModified()) ||
	   (d_coord.get() && d_coord.get()->isModified()) ||
	   (d_normal.get() && d_normal.get()->isModified()) ||
	   (d_texCoord.get() && d_texCoord.get()->isModified()) );
}

void VrmlNodeIFaceSet::updateModified(VrmlNodePath& path, int flags)
{
  //cout << "VrmlNodeIFaceSet::updateModified()" << endl;
  if (this->isModified()) markPathModified(path, true, flags);
  path.push_front(this);
  if (d_color.get()) d_color.get()->updateModified(path, flags);
  if (d_coord.get()) d_coord.get()->updateModified(path, flags);
  if (d_normal.get()) d_normal.get()->updateModified(path, flags);
  if (d_texCoord.get()) d_texCoord.get()->updateModified(path, flags);
  path.pop_front();
}

void VrmlNodeIFaceSet::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_color.get()) d_color.get()->clearFlags();
  if (d_coord.get()) d_coord.get()->clearFlags();
  if (d_normal.get()) d_normal.get()->clearFlags();
  if (d_texCoord.get()) d_texCoord.get()->clearFlags();
}

void VrmlNodeIFaceSet::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  if (d_color.get()) d_color.get()->addToScene(s, rel);
  if (d_coord.get()) d_coord.get()->addToScene(s, rel);
  if (d_normal.get()) d_normal.get()->addToScene(s, rel);
  if (d_texCoord.get()) d_texCoord.get()->addToScene(s, rel);
}

void VrmlNodeIFaceSet::copyRoutes( VrmlNamespace *ns ) const
{
  VrmlNode::copyRoutes(ns);
  if (d_color.get()) d_color.get()->copyRoutes(ns);
  if (d_coord.get()) d_coord.get()->copyRoutes(ns);
  if (d_normal.get()) d_normal.get()->copyRoutes(ns);
  if (d_texCoord.get()) d_texCoord.get()->copyRoutes(ns);
}


ostream& VrmlNodeIFaceSet::printFields(ostream& os, int indent)
{
  if (! d_ccw.get()) PRINT_FIELD(ccw);
  if (! d_convex.get()) PRINT_FIELD(convex);
  if (! d_normalPerVertex.get()) PRINT_FIELD(normalPerVertex);
  if (! d_solid.get()) PRINT_FIELD(solid);

  if (d_creaseAngle.get() != 0.0) PRINT_FIELD(creaseAngle);
  if (d_normal.get()) PRINT_FIELD(normal);
  if (d_normalIndex.getLength() > 0) PRINT_FIELD(normalIndex);
  if (d_texCoord.get()) PRINT_FIELD(texCoord);
  if (d_texCoordIndex.getLength() > 0) PRINT_FIELD(texCoordIndex);

  VrmlNodeIndexedSet::printFields(os, indent);

  return os;
}
  

// TO DO: stripify, crease angle, generate normals ...

Viewer::Object VrmlNodeIFaceSet::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;

  if (rc.getDrawBSpheres()) {
    const VrmlBSphere* bs = (VrmlBSphere*)this->getBVolume();
    viewer->drawBSphere(*bs, 4);
  }

  if (d_coord.get() && d_coordIndex.getLength() > 0)
    {
      VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();
      int nvert = coord.getLength();
      const float *tc = 0, *color = 0, *normal = 0;
      int ntc = 0;
      size_t ntci = 0; const long * tci = 0;	// texture coordinate indices
      int nci = 0; const long * ci = 0;	// color indices
      int nni = 0; const long * ni = 0;	// normal indices

      // Get texture coordinates and texCoordIndex
      if (d_texCoord.get())
	{
	  VrmlMFVec2f &texcoord =
	    d_texCoord.get()->toTextureCoordinate()->coordinate();
	  tc = &texcoord[0][0];
	  ntc = texcoord.getLength();
	  ntci = d_texCoordIndex.getLength();
	  if (ntci) tci = d_texCoordIndex.get();
	}

      // check #tc is consistent with #coords/max texCoordIndex...
      if (tci && ntci < d_coordIndex.getLength())
	{
	  theSystem->error("IndexedFaceSet: not enough texCoordIndex values (there should be at least as many as coordIndex values).\n");
	  theSystem->error("IndexedFaceSet: #coord %d, #coordIndex %d, #texCoord %d, #texCoordIndex %d\n", nvert, d_coordIndex.getLength(), ntc, ntci);
	  tci = 0;
	  ntci = 0;
	}

      // check #colors is consistent with colorPerVtx, colorIndex...
      if (d_color.get())
	{
	  VrmlMFColor &c = d_color.get()->toColor()->color();
	  color = &c[0][0];
	  nci = d_colorIndex.getLength();
	  if (nci) ci = d_colorIndex.get();
	}

      // check #normals is consistent with normalPerVtx, normalIndex...
      if (d_normal.get())
	{
	  VrmlMFVec3f &n = d_normal.get()->toNormal()->normal();
	  normal = &n[0][0];
	  nni = d_normalIndex.getLength();
	  if (nni) ni = d_normalIndex.get();
	}

      unsigned int optMask = 0;
      if (d_ccw.get()) optMask |= Viewer::MASK_CCW;
      if (d_convex.get()) optMask |= Viewer::MASK_CONVEX;
      if (d_solid.get()) optMask |= Viewer::MASK_SOLID;
      if (d_colorPerVertex.get()) optMask |= Viewer::MASK_COLOR_PER_VERTEX;
      if (d_normalPerVertex.get()) optMask |= Viewer::MASK_NORMAL_PER_VERTEX;

      obj = viewer->insertShell(optMask,
				nvert, &coord[0][0],
				d_coordIndex.getLength(), d_coordIndex.get(),
				tc, ntci, tci,
				normal, nni, ni,
				color, nci, ci);
    }

  if (d_color.get()) d_color.get()->clearModified();
  if (d_coord.get()) d_coord.get()->clearModified();
  if (d_normal.get()) d_normal.get()->clearModified();
  if (d_texCoord.get()) d_texCoord.get()->clearModified();

  return obj;
}

// Get the value of one of the exposedFields

const VrmlField *VrmlNodeIFaceSet::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "normal" ) == 0 )
    return &d_normal;
  else if ( strcmp( fieldName, "texCoord" ) == 0 )
    return &d_texCoord;
  
  return VrmlNodeIndexedSet::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeIFaceSet::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_FIELD(ccw, SFBool)
  else if TRY_FIELD(convex, SFBool)
  else if TRY_FIELD(creaseAngle, SFFloat)
  else if TRY_SFNODE_FIELD(normal, Normal)
  else if TRY_FIELD(normalIndex, MFInt32)
  else if TRY_FIELD(normalPerVertex, SFBool)
  else if TRY_FIELD(solid, SFBool)
  else if TRY_FIELD(normalIndex, MFInt32)
  else if TRY_SFNODE_FIELD(texCoord, TextureCoordinate)
  else if TRY_FIELD(texCoordIndex, MFInt32)
  else
    VrmlNodeIndexedSet::setField(fieldName, fieldValue);

  // overly conservative: changing the creaseAngle doesn't really
  // change the bvolume, but the TRY_FIELD macro makes it hard to
  // case things out. sigh. "clarity over optimization", but it
  // still rankles...
  //
  this->setBVolumeDirty(true);
}


VrmlNode* VrmlNodeIFaceSet::getNormal()
{   return d_normal.get(); }

const VrmlMFInt32 & VrmlNodeIFaceSet::getNormalIndex() const
{   return d_normalIndex; }

VrmlNode* VrmlNodeIFaceSet::getTexCoord()
{   return d_texCoord.get(); }

const VrmlMFInt32 & VrmlNodeIFaceSet::getTexCoordIndex() const
{   return d_texCoordIndex; }

VrmlNodeIFaceSet* VrmlNodeIFaceSet::toIFaceSet() const
{ return (VrmlNodeIFaceSet*) this; }



void VrmlNodeIFaceSet::recalcBSphere()
{
  //cout << "VrmlNodeIFaceSet::recalcBSphere()" << endl;

  // take the bvolume of all the points. technically, we should figure
  // out just which points are used by the index and just use those,
  // but for a first pass this is fine (also: if we do it this way
  // then we don't have to update the bvolume when the index
  // changes). motto: always do it the simple way first...
  //
  VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();
  const float* p = coord.get();
  int n = coord.getLength();
  d_bsphere.reset();
  d_bsphere.enclose(p, n);
  //int nvert = coord.size();
  //for(int i=0; i<nvert; i++) {
  //float* vi = coord[i]; // vi[3]
  //d_bsphere.extend(vi);
  //}
  //d_bsphere.dump(cout);
  this->setBVolumeDirty(false);
}


const VrmlBVolume* VrmlNodeIFaceSet::getBVolume() const
{
  //cout << "VrmlNodeIFaceSet::getBVolume():" << this->isBVolumeDirty() << endl;
  if (this->isBVolumeDirty())
    ((VrmlNodeIFaceSet*)this)->recalcBSphere();
  return &d_bsphere; // hmmm, const?
}