//
//  VRORenderTargetOpenGL.h
//  ViroKit
//
//  Created by Raj Advani on 8/9/17.
//  Copyright © 2017 Viro Media. All rights reserved.
//

#ifndef VRORenderTargetOpenGL_h
#define VRORenderTargetOpenGL_h

#include "VRORenderTarget.h"
#include "VROOpenGL.h"

class VRODriver;
class VRODriverOpenGL;

class VRORenderTargetOpenGL : public VRORenderTarget {
    
public:
    
    /*
     Create a new render-target of the given type. The number of images is only required
     for array types.
     */
    VRORenderTargetOpenGL(VRORenderTargetType type, int numAttachments, int numImages,
                          bool enableMipmaps, std::shared_ptr<VRODriverOpenGL> driver);
    virtual ~VRORenderTargetOpenGL();
    
#pragma mark - VRORenderTarget Implementation
    
    void bind();
    virtual void invalidate();
    virtual void blitColor(std::shared_ptr<VRORenderTarget> destination, bool flipY,
                           std::shared_ptr<VRODriver> driver);
    virtual void blitStencil(std::shared_ptr<VRORenderTarget> destination, bool flipY,
                             std::shared_ptr<VRODriver> driver);
    
    virtual bool setViewport(VROViewport viewport);
    int getWidth() const;
    int getHeight() const;
    
#pragma mark - Render Target Setup
    
    virtual bool hasTextureAttached(int attachment);
    virtual void clearTextures();
    virtual bool attachNewTextures();
    virtual void attachTexture(std::shared_ptr<VROTexture> texture, int attachment);
    virtual void setTextureImageIndex(int index, int attachment);
    virtual void setTextureCubeFace(int face, int mipLevel, int attachmentIndex);
    virtual void setMipLevel(int mipLevel, int attachmentIndex);
    virtual const std::shared_ptr<VROTexture> getTexture(int attachment) const;
    virtual void deleteFramebuffers();
    virtual bool restoreFramebuffers();
    
#pragma mark - Render Target Rendering
    
    void clearStencil(int bits);
    void clearDepth();
    void clearColor();
    void clearDepthAndColor();
    void enablePortalStencilWriting(VROFace face);
    void enablePortalStencilRemoval(VROFace face);
    void disablePortalStencilWriting(VROFace face);
    void setPortalStencilPassFunction(VROFace face, VROStencilFunc func, int ref);
    
protected:
    
    /*
     The OpenGL ES names for the framebuffer and depth/stencil buffer(s) used to render to
     this render-target. 0 for those that are not used.
     */
    GLuint _framebuffer, _depthStencilbuffer;
    
    /*
     The viewport of this target.
     */
    VROViewport _viewport;
    
private:
    
#pragma mark - Private
    
    /*
     The colorbuffers are used for pure offscreen rendering and for MSAA texture rendering.
     The textures are used for render-to-texture targets. 0 for those not used.
     */
    GLuint _colorbuffer;
    std::vector<std::shared_ptr<VROTexture>> _textures;
    
    /*
     If this is an array type, indicates the number of images in the texture.
     */
    int _numImages;
    
    /*
     If true, the color textures will have mipmap storage allocated so we can write to
     specific miplevels. To set the active miplevel use either setMipLevel or
     setTextureCubeFace.
     */
    bool _mipmapsEnabled;
    
    /*
     The setting for passing the stencil test operation. These are determined by the
     active portal settings.
     */
    int _stencilRef;
    VROStencilFunc _stencilFunc;
    
    /*
     The storage type used for the depth/stencil renderbuffer (e.g. GL_DEPTH24_STENCIL8).
     */
    GLenum _depthStencilRenderbufferStorage;
    
    /*
     Get the underlying OpenGL target and texture name for the currently attached
     texture.
     */
    GLint getTextureName(int attachment) const;
    
    /*
     Get the attachment type used by the texture (e.g. GL_COLOR_ATTACHMENT0, etc.).
     */
    GLenum getTextureAttachmentType(int attachment) const;
    
    /*
     The driver that created this render target.
     */
    std::weak_ptr<VRODriverOpenGL> _driver;
    
    /*
     Create color and depth render buffers.
     */
    void createColorDepthRenderbuffers();
    
    /*
     Create a color render-to-texture target with a depth render buffer.
     */
    bool createColorTextureTarget();
    
    /*
     Create a depth render-to-texture target with a color render buffer.
     */
    bool createDepthTextureTarget();
    
    /*
     Blit the given attachment to the given destination target.
     */
    void blitAttachment(GLenum attachment, GLbitfield mask, GLenum filter,
                        std::shared_ptr<VRORenderTarget> destination,
                        bool flipY, std::shared_ptr<VRODriver> driver);
    
    
};

#endif /* VRORenderTargetOpenGL_h */
