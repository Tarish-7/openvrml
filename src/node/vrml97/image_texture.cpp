// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
// Copyright 2002  S. K. Bose
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

# include "image_texture.h"
# include "abstract_texture.h"
# include "image_stream_listener.h"
# include <private.h>
# include <openvrml/scene.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL image_texture_node :
        public openvrml_node_vrml97::abstract_texture_node<image_texture_node> {

        friend class openvrml_node_vrml97::image_texture_metatype;

        class url_exposedfield : public exposedfield<openvrml::mfstring> {
        public:
            explicit url_exposedfield(image_texture_node & node);
            url_exposedfield(const url_exposedfield & obj) OPENVRML_NOTHROW;
            virtual ~url_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::mfstring & url,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        url_exposedfield url_;

        boost::shared_mutex image_mutex_;
        openvrml::image image_;
        bool texture_needs_update;

    public:
        image_texture_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~image_texture_node() OPENVRML_NOTHROW;

    private:
        virtual const openvrml::image & do_image() const OPENVRML_NOTHROW;
        virtual void do_render_texture(openvrml::viewer & v);

        void update_texture();
    };

    /**
     * @class image_texture_node
     *
     * @brief Represents ImageTexture node instances.
     */

    /**
     * @var class image_texture_node::image_texture_metatype
     *
     * @brief Class object for ImageTexture nodes.
     */

    /**
     * @internal
     *
     * @class image_texture_node::url_exposedfield
     *
     * @brief url exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param node  image_texture_node.
     */
    image_texture_node::url_exposedfield::
    url_exposedfield(image_texture_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        mfstring_listener(node),
        exposedfield<mfstring>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    image_texture_node::url_exposedfield::
    url_exposedfield(const url_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        mfstring_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::mfstring>(obj)
    {}

    /**
     * @brief Destroy.
     */
    image_texture_node::url_exposedfield::
    ~url_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    image_texture_node::url_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new url_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param url       url.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    image_texture_node::url_exposedfield::
    event_side_effect(const openvrml::mfstring & url, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            image_texture_node & image_texture =
                dynamic_cast<image_texture_node &>(
                    this->node_event_listener::node());

            image_texture.url_.mfstring::value(url.value());
            image_texture.texture_needs_update = true;
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var image_texture_node::url_exposedfield image_texture_node::url_
     *
     * @brief url exposedField.
     */

    /**
     * @var openvrml::image image_texture_node::image_
     *
     * @brief Image data.
     */

    /**
     * @var bool image_texture_node::texture_needs_update
     *
     * @brief Flag to indicate that image_texture_node::update_texture should
     *        be called.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope the scope to which the node belongs.
     */
    image_texture_node::
    image_texture_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml_node_vrml97::abstract_texture_node<image_texture_node>(
            type, scope),
        url_(*this),
        texture_needs_update(true)
    {}

    /**
     * @brief Destroy.
     */
    image_texture_node::~image_texture_node() OPENVRML_NOTHROW
    {
        // delete texObject...
    }

    /**
     * @brief The image.
     *
     * @return the image.
     */
    const openvrml::image &
    image_texture_node::do_image() const OPENVRML_NOTHROW
    {
        return this->image_;
    }

    /**
     * @brief render_texture implementation.
     *
     * @param v viewer.
     *
     * @return object identifier for the inserted texture.
     */
    void image_texture_node::do_render_texture(openvrml::viewer & v)
    {
        this->update_texture();
        boost::shared_lock<boost::shared_mutex> lock(this->image_mutex_);
        v.insert_texture(*this, true);
    }

    /**
     * @brief Called to update texture data lazily.
     */
    void image_texture_node::update_texture()
    {
        assert(this->scene());

        if (this->texture_needs_update) {
            using openvrml_node_vrml97::image_stream_listener;
            try {
                if (!this->url_.value().empty()) {
                    using std::auto_ptr;
                    auto_ptr<openvrml::resource_istream> in =
                        this->scene()->get_resource(this->url_.value());
                    if (*in) {
                        auto_ptr<openvrml::stream_listener> listener(
                            new image_stream_listener(in->url(),
                                                      this->image_,
                                                      *this,
                                                      this->image_mutex_));
                        this->scene()->read_stream(in, listener);
                    }
                }
            } catch (std::exception & ex) {
                this->scene()->browser().err(ex.what());
            }
            this->texture_needs_update = false;
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::image_texture_metatype::id =
    "urn:X-openvrml:node:ImageTexture";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::image_texture_metatype::
image_texture_metatype(openvrml::browser & browser):
    node_metatype(image_texture_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::image_texture_metatype::~image_texture_metatype()
    OPENVRML_NOTHROW
{}

# define IMAGE_TEXTURE_INTERFACE_SEQ                  \
    ((exposedfield, mfstring, "url",      url_))      \
    ((field,        sfbool,   "repeatS",  repeat_s_)) \
    ((field,        sfbool,   "repeatT",  repeat_t_)) \
    ((exposedfield, sfnode,   "metadata", metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              image_texture_metatype,
                                              image_texture_node,
                                              IMAGE_TEXTURE_INTERFACE_SEQ)
