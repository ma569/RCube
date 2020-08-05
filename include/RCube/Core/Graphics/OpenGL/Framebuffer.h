#pragma once

#include "RCube/Core/Graphics/OpenGL/Texture.h"
#include "glad/glad.h"
#include <vector>

namespace rcube
{

/**
 * Framebuffer represents a collection of color, depth and stencil buffers that can be
 * used for off-screen rendering.
 * Use the Framebuffer::create(...) static method to create a new framebuffer.
 */
class Framebuffer
{
  public:
    Framebuffer();
    Framebuffer(const Framebuffer &other) = delete;
    ~Framebuffer();
    /**
     * Creates a new framebuffer
     * @param width Width of color and depth buffers
     * @param height Height of color and depth buffers
     * @return Shared pointer to Framebuffer
     */
    static std::shared_ptr<Framebuffer> create();
    /**
     * Returns whether the Framebuffer is actually created and initialized on the GPU
     * @return Whether initialized on GPU
     */
    bool initialized() const;
    /**
     * Releases the resources associated to the framebuffer from the GPU
     */
    void release();
    /**
     * Bind and use the framebuffer
     */
    void use();
    /**
     * Bind and use the framebuffer for writing
     */
    void useForWrite();
    /**
     * Bind and use the framebuffer for reading
     */
    void useForRead();
    /**
     * Unbind the framebuffer
     */
    void done() const;
    /**
     * OpenGL ID of the framebuffer
     * @return
     */
    GLuint id() const;

    void setDrawBuffers(const std::vector<int> &attachment_indices);

    void setReadBuffer(int attachment_index);

    void setColorAttachment(size_t index, std::shared_ptr<Texture2D> tex);

    void resize(size_t width, size_t height);

    void setDepthAttachment(std::shared_ptr<Texture2D> tex);

    /**
     * Add/replace the depth-stencil attachment
     */
    void setDepthStencilAttachment(std::shared_ptr<Texture2D> tex);

    /**
     * Returns the ith color attachment
     * @param i Index of the color attachment
     * @return Pointer to Texture2D
     */
    std::shared_ptr<Texture2D> colorAttachment(size_t i = 0);

    /**
     * Returns the number of color attachments in this framebuffer
     * @return Number of color attachments
     */
    size_t numColorAttachments() const;

    /**
     * Returns whether there is depth/stencil attachment in this framebuffer
     * @return
     */
    bool hasDepthStencilAttachment() const;

    /**
     * Returns whether the framebuffer is completely defined
     * @return Whether complete
     */
    bool isComplete();

    /**
     * Returns an image representing the color attachment
     * @param attachment_index Index of the color attachment
     * @return Image
     */
    Image getImage(int attachment_index = 0) const;
    /**
     * Blits (copies) data from the current framebuffer to the target
     * Only works if both framebuffers match in dimensions
     * @param target_fbo
     */
    void blit(std::shared_ptr<Framebuffer> target_fbo, glm::ivec2 src0, glm::ivec2 src1,
              glm::ivec2 dst0, glm::ivec2 dst1, bool color = true, bool depth = true,
              bool stencil = true);

    /**
     * Blits (copies) data from the current framebuffer to the default framebuffer
     * @param dst0 ROI denoted by bottom-right point in current framebuffer
     * @param dst1 ROI denoted by bottom-right point in destination framebuffer
     * @param color Whether to copy color buffers
     * @param depth Whether to copy depth buffer
     * @param stencil Whether to copy stencil buffer
     */

    void blitToScreen(glm::ivec2 src0, glm::ivec2 src1, glm::ivec2 dst0, glm::ivec2 dst1,
                      bool color = true, bool depth = true,
                      bool stencil = true);
    
    void copySubImage(int attachment_index, std::shared_ptr<TextureCubemap> output,
                      TextureCubemap::Side face, int level, glm::ivec2 src_origin,
                      glm::ivec2 src_size);

  private:
    unsigned int id_ = 0;
    std::vector<std::shared_ptr<Texture2D>> colors_;
    std::shared_ptr<Texture2D> depth_stencil_;
};

} // namespace rcube
