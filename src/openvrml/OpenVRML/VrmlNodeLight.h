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

#ifndef VRMLNODELIGHT_H
#define VRMLNODELIGHT_H

#include "VrmlNodeChild.h"
#include "field.h"

class VrmlNodeLight : public VrmlNodeChild {

public:

  // Define the fields of light nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeLight(VrmlScene *);
  virtual ~VrmlNodeLight();

  virtual VrmlNodeLight* toLight() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void renderScoped(Viewer *);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName,
			const VrmlField &fieldValue);

  virtual float getAmbientIntensity() const { return d_ambientIntensity.get(); } //LarryD Mar 04/99
  virtual float getIntensity() const { return d_intensity.get(); } //LarryD Mar 04/99
  virtual bool  getOn() const { return d_on.get(); } //LarryD Mar 04/99
  virtual const float *getColor() const { return d_color.get(); }  //LarryD Mar 04/99

protected:

  VrmlSFFloat d_ambientIntensity;
  VrmlSFColor d_color;
  VrmlSFFloat d_intensity;
  VrmlSFBool d_on;
};

#endif
