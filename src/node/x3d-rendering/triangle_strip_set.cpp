// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008, 2009  Braden McDaniel
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

# include "triangle_strip_set.h"
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
     * @brief Represents TriangleStripSet node instances.
     */
    class OPENVRML_LOCAL triangle_strip_set_node :
        public abstract_node<triangle_strip_set_node>,
        public geometry_node,
        public child_node {

        friend class openvrml_node_x3d_rendering::triangle_strip_set_metatype;

        exposedfield<sfnode> color_;
        exposedfield<sfnode> coord_;
        exposedfield<sfnode> normal_;
        exposedfield<mfint32> strip_count_;
        exposedfield<sfnode> tex_coord_;
        sfbool ccw_;
        sfbool color_per_vertex_;
        sfbool normal_per_vertex_;
        sfbool solid_;
        bounding_sphere bsphere;

    public:
        triangle_strip_set_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~triangle_strip_set_node() OPENVRML_NOTHROW;

        virtual const color_node * color() const OPENVRML_NOTHROW;

    private:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);

        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        rendering_context context);
        virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

        void recalc_bsphere();
    };


    /**
     * @var triangle_strip_set_node::triangle_strip_set_metatype
     *
     * @brief Class object for TriangleStripSet nodes.
     */

    /**
     * @var triangle_strip_set_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var triangle_strip_set_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var triangle_strip_set_node::normal_
     *
     * @brief normal exposedField
     */

    /**
     * @var triangle_strip_set_node::strip_count_
     *
     * @brief strip_count exposedField
     */

    /**
     * @var triangle_strip_set_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var triangle_strip_set_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var triangle_strip_set_node::color_per_vertex_
     *
     * @brief color_per_vertex field
     */

    /**
     * @var triangle_strip_set_node::normal_per_vertex_
     *
     * @brief normal_per_vertex field
     */

    /**
     * @var triangle_strip_set_node::solid_
     *
     * @brief solid field
     */


    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    triangle_strip_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<self_t *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void triangle_strip_set_node::recalc_bsphere()
    {
        // take the bvolume of all the points. technically, we should figure
        // out just which points are used by the index and just use those,
        // but for a first pass this is fine (also: if we do it this way
        // then we don't have to update the bvolume when the index
        // changes). motto: always do it the simple way first...
        //
        openvrml::coordinate_node * const coordinateNode =
            node_cast<openvrml::coordinate_node *>(
                this->coord_.sfnode::value().get());
        if (coordinateNode) {
            const std::vector<vec3f> & coord = coordinateNode->point();
            this->bsphere = bounding_sphere();
            this->bsphere.enclose(coord);
        }
        this->bounding_volume_dirty(false);
    }

    const color_node * triangle_strip_set_node::color() const OPENVRML_NOTHROW
    {
        return node_cast<color_node *>(color_.sfnode::value().get());
    }

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    void
    triangle_strip_set_node::
    do_render_geometry(openvrml::viewer & /* viewer */,
                       const rendering_context /* context */)
    {}


    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool triangle_strip_set_node::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        return (this->color_.value() && this->color_.value()->modified())
            || (this->coord_.value() && this->coord_.value()->modified())
            || (this->normal_.value() && this->normal_.value()->modified())
            || (this->tex_coord_.value()
                && this->tex_coord_.value()->modified());
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    triangle_strip_set_node::
    triangle_strip_set_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        color_(*this),
        coord_(*this),
        normal_(*this),
        strip_count_(*this),
        tex_coord_(*this),
        ccw_(true),
        color_per_vertex_(true),
        normal_per_vertex_(true),
        solid_(true)
    {}

    /**
     * @brief Destroy.
     */
    triangle_strip_set_node::~triangle_strip_set_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_rendering::triangle_strip_set_metatype::id =
    "urn:X-openvrml:node:TriangleStripSet";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c triangle_strip_set_metatype.
 */
openvrml_node_x3d_rendering::triangle_strip_set_metatype::
triangle_strip_set_metatype(openvrml::browser & browser):
    node_metatype(triangle_strip_set_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_rendering::triangle_strip_set_metatype::
~triangle_strip_set_metatype()
    OPENVRML_NOTHROW
{}

# define TRIANGLE_STRIP_SET_INTERFACE_SEQ                            \
    ((exposedfield, sfnode,  "metadata",        metadata))           \
    ((exposedfield, sfnode,  "color",           color_))             \
    ((exposedfield, sfnode,  "coord",           coord_))             \
    ((exposedfield, sfnode,  "normal",          normal_))            \
    ((exposedfield, mfint32, "stripCount",      strip_count_))       \
    ((exposedfield, sfnode,  "texCoord",        tex_coord_))         \
    ((field,        sfbool,  "ccw",             ccw_))               \
    ((field,        sfbool,  "colorPerVertex",  color_per_vertex_))  \
    ((field,        sfbool,  "normalPerVertex", normal_per_vertex_)) \
    ((field,        sfbool,  "solid",           solid_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_rendering,
                                              triangle_strip_set_metatype,
                                              triangle_strip_set_node,
                                              TRIANGLE_STRIP_SET_INTERFACE_SEQ)
