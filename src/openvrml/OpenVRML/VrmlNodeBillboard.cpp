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
//  VrmlNodeBillboard.cpp

#include "VrmlNodeBillboard.h"
#include "MathUtils.h"
#include "VrmlNodeType.h"


static VrmlNode *creator(VrmlScene *s) { return new VrmlNodeBillboard(s); }


// Define the built in VrmlNodeType:: "Billboard" fields

VrmlNodeType *VrmlNodeBillboard::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Billboard", creator);
      t->reference();
    }

  VrmlNodeGroup::defineType(t);	// Parent class
  t->addExposedField("axisOfRotation", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeBillboard::nodeType() const
{
    return *defineType(0);
}


VrmlNodeBillboard::VrmlNodeBillboard(VrmlScene *scene) :
  VrmlNodeGroup(scene),
  d_axisOfRotation(0.0, 1.0, 0.0),
  d_xformObject(0)
{
}

VrmlNodeBillboard::~VrmlNodeBillboard()
{
  // delete d_xformObject...
}

VrmlNode *VrmlNodeBillboard::cloneMe() const
{
  return new VrmlNodeBillboard(*this);
}


ostream& VrmlNodeBillboard::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_axisOfRotation.getX()) ||
      ! FPZERO(d_axisOfRotation.getY()) ||
      ! FPZERO(d_axisOfRotation.getZ()) )
    PRINT_FIELD(axisOfRotation);

  VrmlNodeGroup::printFields(os, indent);
  return os;
}


void VrmlNodeBillboard::render(Viewer *viewer, VrmlRenderContext rc)
{
  if ( d_xformObject && isModified() )
    {
      viewer->removeObject(d_xformObject);
      d_xformObject = 0;
    }

  if (d_xformObject)
    viewer->insertReference(d_xformObject);

  else if (d_children.getLength() > 0)
    {
      d_xformObject = viewer->beginObject(name());

      viewer->setBillboardTransform( d_axisOfRotation.get() );

      // Render children
      VrmlNodeGroup::render(viewer, rc);

      viewer->unsetBillboardTransform( d_axisOfRotation.get() );

      viewer->endObject();
    }

  clearModified();
}

// Cache a pointer to (one of the) parent transforms for proper
// rendering of bindables.

void VrmlNodeBillboard::accumulateTransform( VrmlNode *parent )
{
  d_parentTransform = parent;

  int i, n = d_children.getLength();

  for (i = 0; i<n; ++i)
    {
      VrmlNode *kid = d_children[i];
      kid->accumulateTransform( this );
    }
}

VrmlNode* VrmlNodeBillboard::getParentTransform() { return d_parentTransform; }

void VrmlNodeBillboard::inverseTransform(Viewer *viewer)
{
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(viewer);

  // Apply inverted bb transforms...
  //viewer->setBillboardTransform( d_axisOfRotation.get() );
}

void VrmlNodeBillboard::inverseTransform(double m[4][4])
{
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(m);
  else
    Midentity(m);

  // Invert bb transform...
  // ...
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeBillboard::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "axisOfRotation" ) == 0 )
    return &d_axisOfRotation;

  return VrmlNodeGroup::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeBillboard::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(axisOfRotation, SFVec3f)
  else
    VrmlNodeGroup::setField(fieldName, fieldValue);
}
