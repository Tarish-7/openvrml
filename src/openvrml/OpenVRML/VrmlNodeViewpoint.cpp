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

#include "VrmlNodeViewpoint.h"
#include "MathUtils.h"
#include "VrmlNodeType.h"
#include "VrmlScene.h"
#include "Viewer.h"
#include "VrmlFrustum.h"


//  Viewpoint factory.

static VrmlNode *creator(VrmlScene *scene)
{
  return new VrmlNodeViewpoint(scene);
}


// Define the built in VrmlNodeType:: "Viewpoint" fields

VrmlNodeType *VrmlNodeViewpoint::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Viewpoint", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_bind", VrmlField::SFBOOL);
  t->addExposedField("fieldOfView", VrmlField::SFFLOAT);
  t->addExposedField("jump", VrmlField::SFBOOL);
  t->addExposedField("orientation", VrmlField::SFROTATION);
  t->addExposedField("position", VrmlField::SFVEC3F);
  t->addField("description", VrmlField::SFSTRING);
  t->addEventOut("bindTime", VrmlField::SFTIME);
  t->addEventOut("isBound", VrmlField::SFBOOL);

  return t;
}

VrmlNodeType & VrmlNodeViewpoint::nodeType() const
{
    return *defineType(0);
}


static const float DEFAULT_FIELD_OF_VIEW = 0.785398;

VrmlNodeViewpoint::VrmlNodeViewpoint(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_fieldOfView(DEFAULT_FIELD_OF_VIEW),
  d_jump(true),
  d_orientation(0.0, 0.0, 1.0, 0.0),
  d_position(0.0, 0.0, 10.0),
  d_isBound(false),
  d_bindTime(0),
  d_parentTransform(0)
{
  if (d_scene) d_scene->addViewpoint(this);
}

// need copy constructor for d_parentTransform ...

VrmlNodeViewpoint::~VrmlNodeViewpoint()
{
  if (d_scene) d_scene->removeViewpoint(this);
}


VrmlNode *VrmlNodeViewpoint::cloneMe() const
{
  return new VrmlNodeViewpoint(*this);
}


VrmlNodeViewpoint* VrmlNodeViewpoint::toViewpoint() const
{ return (VrmlNodeViewpoint*) this; }

void VrmlNodeViewpoint::addToScene(VrmlScene *s, const char *)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addViewpoint(this); }


ostream& VrmlNodeViewpoint::printFields(ostream& os, int indent)
{
  if (! FPEQUAL( d_fieldOfView.get(), DEFAULT_FIELD_OF_VIEW))
    PRINT_FIELD(fieldOfView);
  if (! d_jump.get()) PRINT_FIELD(jump);
  if (! FPZERO(d_orientation.getX()) ||
      ! FPZERO(d_orientation.getY()) ||
      ! FPEQUAL(d_orientation.getZ(), 1.0) ||
      ! FPZERO(d_orientation.getAngle()) )
    PRINT_FIELD(orientation);
  if (! FPZERO(d_position.getX()) ||
      ! FPZERO(d_position.getY()) ||
      ! FPEQUAL(d_position.getZ(), 10.0) )
    PRINT_FIELD(position);
  if (d_description.get()) PRINT_FIELD(description);

  return os;
}

// Cache a pointer to (one of the) parent transforms for proper
// rendering of bindables.

void VrmlNodeViewpoint::accumulateTransform( VrmlNode *parent )
{
  d_parentTransform = parent;
}

VrmlNode* VrmlNodeViewpoint::getParentTransform() { return d_parentTransform; }

// Note that this method is not maintaining isBound.

void VrmlNodeViewpoint::eventIn(double timeStamp,
				const char *eventName,
				const VrmlField *fieldValue)
{
  if (strcmp(eventName, "set_bind") == 0)
    {
      VrmlNodeViewpoint *current = d_scene->bindableViewpointTop();
      const VrmlSFBool *b = fieldValue->toSFBool();
      
      if (! b)
	{
	  cerr << "Error: invalid value for Viewpoint::set_bind eventIn "
	       << (*fieldValue) << endl;
	  return;
	}

      if ( b->get() )		// set_bind TRUE
	{
	  if (this != current)
	    {
	      if (current)
		current->eventOut( timeStamp, "isBound", VrmlSFBool(false));
	      d_scene->bindablePush( this );
	      eventOut( timeStamp, "isBound", VrmlSFBool(true) );
	      const char *n = name();
	      const char *d = d_description.get();
	      if ( *n && d && *d )
		theSystem->inform("%s: %s", n, d);
	      else if ( d && *d )
		theSystem->inform("%s", d);
	      else if ( *n )
		theSystem->inform("%s", n);
	    }
	}
      else			// set_bind FALSE
	{
	  d_scene->bindableRemove( this );
	  if (this == current)
	    {
	      eventOut( timeStamp, "isBound", VrmlSFBool(false));
	      current = d_scene->bindableViewpointTop();
	      if (current)
		current->eventOut( timeStamp, "isBound", VrmlSFBool(true) );
	    }
	}

      d_bindTime.set( timeStamp );
      eventOut( timeStamp, "bindTime", d_bindTime );
    }

  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);
    }
}


// Get the value of a field or eventOut.
// The isBound eventOut is only set when queried,
// don't rely on it's value to be valid. This hoses
// the const-ness of the method, of course :(

const VrmlField *VrmlNodeViewpoint::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "fieldOfView" ) == 0 )
    return &d_fieldOfView;
  else if ( strcmp( fieldName, "jump" ) == 0 )
    return &d_jump;
  else if ( strcmp( fieldName, "orientation" ) == 0 )
    return &d_orientation;
  else if ( strcmp( fieldName, "position" ) == 0 )
    return &d_position;

  // eventOuts
  else if ( strcmp( fieldName, "bindTime" ) == 0 )
    return &d_bindTime;  
  else if ( strcmp( fieldName, "isBound" ) == 0 )
    {
      VrmlSFBool* isBound = (VrmlSFBool*) &(this->d_isBound);
      isBound->set( d_scene->bindableViewpointTop() == this );
      return isBound;
    }

  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeViewpoint::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(fieldOfView, SFFloat)
  else if TRY_FIELD(jump, SFBool)
  else if TRY_FIELD(orientation, SFRotation)
  else if TRY_FIELD(position, SFVec3f)
  else if TRY_FIELD(description, SFString)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


void VrmlNodeViewpoint::getInverseMatrix(double IM[4][4]) const
{
  //cout << "VrmlNodeViewpoint::getInverseMatrix()" << endl;

  double rot_mat[4][4];
  float rot_aa[4];
  rot_aa[0] =  d_orientation.getX();
  rot_aa[1] =  d_orientation.getY();
  rot_aa[2] =  d_orientation.getZ();
  rot_aa[3] = -d_orientation.getAngle();
  Mrotation(rot_mat, rot_aa);

  double pos_mat[4][4];
  float pos_vec[3];
  pos_vec[0] = -d_position.getX();
  pos_vec[1] = -d_position.getY();
  pos_vec[2] = -d_position.getZ();
  Mtranslation(pos_mat, pos_vec);

  //MM(IM, pos_mat, rot_mat);
  MM(IM, rot_mat, pos_mat);
}

void VrmlNodeViewpoint::getFrustum(VrmlFrustum& frust) const
{
  cout << "VrmlNodeViewpoint::getFrustum():WARNING:not implemented" << endl;
}


const VrmlBVolume* VrmlNodeViewpoint::getBVolume() const
{
  //cout << "VrmlNodeViewpoint::getBVolume():unset" << endl;
  static VrmlBSphere* inf_bsphere = (VrmlBSphere*)0;
  if (!inf_bsphere) {
    inf_bsphere = new VrmlBSphere();
  }
  return inf_bsphere;
}

const VrmlSFFloat & VrmlNodeViewpoint::getFieldOfView() const {
    return this->d_fieldOfView;
}

const VrmlSFRotation & VrmlNodeViewpoint::getOrientation() const {
    return this->d_orientation;
}

const VrmlSFVec3f & VrmlNodeViewpoint::getPosition() const {
    return this->d_position;
}

const VrmlSFString & VrmlNodeViewpoint::getDescription() const {
    return this->d_description;
}