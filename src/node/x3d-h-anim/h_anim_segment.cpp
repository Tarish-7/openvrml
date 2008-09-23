// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# include "h_anim_segment.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Represents HAnimSegment node instances.
     */
    class OPENVRML_LOCAL h_anim_segment_node :
        public abstract_node<h_anim_segment_node>,
        public child_node {

        friend class openvrml_node_x3d_h_anim::h_anim_segment_metatype;

        class add_children_listener : public event_listener_base<self_t>,
                                      public mfnode_listener {
            public:
                explicit add_children_listener(self_t & node);
                virtual ~add_children_listener() OPENVRML_NOTHROW;

            private:
                virtual void do_process_event(const mfnode & fraction,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_children_listener : public event_listener_base<self_t>,
                                         public mfnode_listener {
            public:
                explicit remove_children_listener(self_t & node);
                virtual ~remove_children_listener() OPENVRML_NOTHROW;

            private:
                virtual void do_process_event(const mfnode & fraction,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);
        };

        add_children_listener add_children_listener_;
        remove_children_listener remove_children_listener_;
        exposedfield<sfvec3f> center_of_mass_;
        exposedfield<mfnode> children_;
        exposedfield<sfnode> coord_;
        exposedfield<mfnode> displacers_;
        exposedfield<sffloat> mass_;
        exposedfield<mffloat> moments_of_inertia_;
        exposedfield<sfstring> name_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        h_anim_segment_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~h_anim_segment_node() OPENVRML_NOTHROW;
    };


    /**
     * @var h_anim_segment_node::h_anim_segment_metatype
     *
     * @brief Class object for HAnimSegment nodes.
     */

    /**
     * @var h_anim_segment_node::add_children_listener h_anim_segment_node::add_children_listener_
     *
     * @brief add_children eventIn listener.
     */

    /**
     * @var h_anim_segment_node::remove_children_listener h_anim_segment_node::remove_children_listener_
     *
     * @brief remove_children eventIn listener.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_segment_node>::exposedfield<sfvec3f> h_anim_segment_node::center_of_mass_
     *
     * @brief center_of_mass exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_segment_node>::exposedfield<mfnode> h_anim_segment_node::children_
     *
     * @brief children exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_segment_node>::exposedfield<sfnode> h_anim_segment_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_segment_node>::exposedfield<mfnode> h_anim_segment_node::displacers_
     *
     * @brief displacers exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_segment_node>::exposedfield<sffloat> h_anim_segment_node::mass_
     *
     * @brief mass exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_segment_node>::exposedfield<mffloat> h_anim_segment_node::moments_of_inertia_
     *
     * @brief moments_of_inertia exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_segment_node>::exposedfield<sfstring> h_anim_segment_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var openvrml::sfvec3f h_anim_segment_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var openvrml::sfvec3f h_anim_segment_node::bbox_size_
     *
     * @brief bbox_size field
     */

    h_anim_segment_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    h_anim_segment_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void h_anim_segment_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    h_anim_segment_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    h_anim_segment_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void h_anim_segment_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    h_anim_segment_node::
    h_anim_segment_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        center_of_mass_(*this),
        children_(*this),
        coord_(*this),
        displacers_(*this),
        mass_(*this),
        moments_of_inertia_(*this, vector<float>(9,0.0f)),
        name_(*this),
        bbox_size_(make_vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    h_anim_segment_node::~h_anim_segment_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_h_anim::h_anim_segment_metatype::id =
    "urn:X-openvrml:node:HAnimSegment";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c h_anim_segment_metatype.
 */
openvrml_node_x3d_h_anim::h_anim_segment_metatype::
h_anim_segment_metatype(openvrml::browser & browser):
    node_metatype(h_anim_segment_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_h_anim::h_anim_segment_metatype::~h_anim_segment_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating HAnimSegment nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by @c h_anim_segment_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_h_anim::h_anim_segment_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 12> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "centerOfMass"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "coord"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "displacers"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "mass"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "momentsOfInertia"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };
    typedef node_type_impl<h_anim_segment_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & the_node_type = static_cast<node_type_t &>(*type);

    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::add_children_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::remove_children_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::center_of_mass_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::children_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::coord_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::displacers_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::mass_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::moments_of_inertia_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::name_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::bbox_center_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_segment_node::bbox_size_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
