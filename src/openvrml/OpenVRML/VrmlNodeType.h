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

#ifndef VRMLNODETYPE_H
#define VRMLNODETYPE_H

#include <list>
#include "common.h"
#include "field.h"
#include "VrmlNodePtr.h"

class Doc2;
class VrmlNamespace;
class VrmlScene;

class OPENVRML_SCOPE VrmlNodeType {
public:
    struct NodeFieldRec {
        VrmlNodePtr node;
        std::string fieldName;
    };
    typedef std::list<NodeFieldRec *> ISMap;
    struct ProtoField {
        std::string name;
        VrmlField::VrmlFieldType type;
        VrmlField * defaultValue;
        ISMap thisIS;
    };
    typedef std::list<ProtoField *> FieldList;

private:
    int d_refCount;
    std::string name;
    VrmlNamespace * d_namespace;
    const VrmlMFString * d_url; // Where the EXTERNPROTO could be.
    std::string actualUrl;	// The URL actually used.
    Doc2 *d_relative;
    VrmlMFNode implNodes;
    // Pointer to function to create instances
    VrmlNode* (*d_creator)( VrmlScene* );	
    // Fields defined for this node type
    FieldList d_eventIns;
    FieldList d_eventOuts;
    FieldList d_fields;
    bool d_fieldsInitialized;

public:
    VrmlNodeType(const std::string & name,
	         VrmlNode* (*creator)(VrmlScene *scene) = 0);
    ~VrmlNodeType();

    VrmlNodeType * reference();
    void dereference();
    VrmlNode * newNode(VrmlScene * scene = 0) const;

    const std::string & getName() const;
    VrmlNamespace * getScope() const;
    void setScope(VrmlNamespace & scope);

    // Routines for adding/getting eventIns/Outs/fields to this type
    void addEventIn(const std::string & name, VrmlField::VrmlFieldType type);
    void addEventOut(const std::string & name, VrmlField::VrmlFieldType type);
    void addField(const std::string & name, VrmlField::VrmlFieldType type,
		  const VrmlField * defaultVal = 0);
    void addExposedField(const std::string & name,
                         VrmlField::VrmlFieldType type,
		         const VrmlField * defaultVal = 0);

    const VrmlField * fieldDefault(const std::string & name) const;
    void setFieldDefault(const std::string & name, const VrmlField * value);

    VrmlField::VrmlFieldType hasEventIn(const std::string & id) const;
    VrmlField::VrmlFieldType hasEventOut(const std::string & id) const;
    VrmlField::VrmlFieldType hasField(const std::string & id) const;
    VrmlField::VrmlFieldType hasExposedField(const std::string & id) const;
    VrmlField::VrmlFieldType hasInterface(const std::string & id) const;

    void setUrl(const VrmlMFString & url, const Doc2 * relative = 0);
    void setActualUrl(const std::string & url);
    const std::string & getActualUrl() const;

    // Add a node to a PROTO implementation
    void addNode(VrmlNode & implNode);

    // Add an IS linkage to one of the PROTO interface fields/events.
    void addIS(const std::string & isFieldName, VrmlNode & implNode,
	       const std::string & implFieldName);


    const VrmlMFNode & getImplementationNodes();

    const VrmlNodePtr firstNode() const;

    const ISMap * getFieldISMap(const std::string & fieldName) const;

    FieldList &fields() { return d_fields; }
    FieldList &eventIns() { return d_eventIns; }
    FieldList &eventOuts() { return d_eventOuts; }

private:
    void fetchImplementation();
    VrmlField::VrmlFieldType has(const FieldList &, const std::string & id) const;
};

#endif // VRMLNODETYPE_H
