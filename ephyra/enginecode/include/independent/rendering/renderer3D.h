/** \file renderer3D.h */
#pragma once

#include <vector>

#include "rendererCommon.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "systems/log.h"
#include "indirectBuffer.h"
#include <memory>

namespace Engine
{

	class Renderer3DVertex
	{
	public:
		glm::vec3 m_pos;
		glm::vec3 m_normal;
		glm::vec2 m_uv;

		Renderer3DVertex() = default;
		Renderer3DVertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& uv) : m_pos(pos), m_normal(normal), m_uv(uv) {}

		static vertexBufferLayout s_layout;
	};

	/** \class Material
	*	Holds a shader and the uniform data associated
	*/
	class Material
	{
	public:
		Material(const std::shared_ptr<Shader>& shader, bool batched = true) : m_shader(shader), m_flags(0), m_texture(nullptr), m_tint(glm::vec4(0.f)) 
		{
			if (batched) setFlag(flag_batched);
		}

		Material(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Texture>& texture, const glm::vec4& tint, bool batched = true) : m_shader(shader), m_texture(texture), m_tint(tint)
		{
			if (batched) setFlag(flag_batched);
			setFlag(flag_texture | flag_tint);
		}

		Material(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Texture>& texture, bool batched = true) : m_shader(shader), m_texture(texture), m_tint(glm::vec4(0.0f))
		{
			if (batched) setFlag(flag_batched);
			setFlag(flag_texture);
		}

		Material(const std::shared_ptr<Shader>& shader, const glm::vec4& tint, bool batched = true) : m_shader(shader), m_texture(nullptr), m_tint(tint)
		{
			if (batched) setFlag(flag_batched);
			setFlag(flag_tint);
		}

		inline std::shared_ptr<Shader> getShader() const { return m_shader; } //!< Return Shader
		inline std::shared_ptr<Texture> getTexture() const { return m_texture; } //!< Return Texture

		inline glm::vec4 getTint() const { return m_tint; } //!< Return tint
		inline bool isFlagSet(uint32_t flag) const { return m_flags & flag; } //!< Return if requested is set

		void setTexture(const std::shared_ptr<Texture>& texture) { m_texture = texture; }
		void setTint(const glm::vec4& tint) { m_tint = tint; }

		constexpr static uint32_t flag_batched = 1 << 0; //!< 0000000001
		constexpr static uint32_t flag_texture = 1 << 1; //!< 0000000010
		constexpr static uint32_t flag_tint = 1 << 2; //!< 0000000100
	private:
		uint32_t m_flags = 0; //!< bit field representation of the shader settings
		std::shared_ptr<UniformBuffer> materialUBO;
		std::shared_ptr<Shader> m_shader; //!< The material's shader
		std::shared_ptr<Texture> m_texture; //!< Texture to be applied to the material
		glm::vec4 m_tint; //!< Colour tint to be applied to the geometry
		void setFlag(uint32_t flag) { m_flags = m_flags | flag; }

	};

	struct Geometry
	{
		uint32_t id;
		uint32_t vertexCount;
		uint32_t indexCount;
		uint32_t firstVertex;
		uint32_t firstIndex;
	};

	struct MaterialInfo
	{
		glm::vec3 albedo;
		float metallic;
		float roughness;
		float ao;
	};

	struct Light
	{
		glm::vec3 lightPos;
		glm::vec3 lightColour;
	};

	struct BatchQueueEntry
	{
		Geometry geometry;
		std::shared_ptr<Material> material;
		glm::mat4 model;
		MaterialInfo materialInfo;
	};

	/** \class Renderer3D
	*	A brief class for 3D Geometry Rendering (Instant not Batch)
	*/
	class Renderer3D
	{
	public:
		static void init(uint32_t vertexCapacity, uint32_t indexCapacity, uint32_t batchSize); //!< Init renderer
		static void begin(const SceneWideUniforms& sceneWideUniforms); //!< Start 3d scene
		static void submit(const Geometry& geometry, const std::shared_ptr<Material>& material, const glm::mat4& model, const MaterialInfo& materialInfo); //!< Submit a piece of geometry
		static void end(); //!< End 3D Scene
		static void flush(); //!< Flush All Draw Queues

		static void initShader(std::shared_ptr<Shader> shader); //!< Attach Shader To The Current Render Context
		static bool addGeometry(std::vector<Renderer3DVertex> vertices, std::vector<uint32_t> indices, Geometry& VAO);
	private:
		static void flushBatch();
		static void flushBatchCommands(std::shared_ptr<Shader>& shader, uint32_t instanceCount);

		struct InternalData
		{	
			std::shared_ptr<UniformBuffer> cameraUBO; //!< View and Proj Mats
			std::shared_ptr<UniformBuffer> lightsUBO; //!< Scenewide Lighting Variables
			std::shared_ptr<VertexArray> VAO; //!< All Static Meshes
			std::shared_ptr<IndirectBuffer> commands; //!< Command Buffer
			std::vector<BatchQueueEntry> batchQueue; //!< Queue Waiting To Be Drawn In Batches
			std::vector<DrawElementsIndirectCommand> batchCommands; //!< Queue Of Commands Waiting To Be Called For Batches

			uint32_t batchCapacity = 0;
			uint32_t vertexCapacity = 0;
			uint32_t indexCapacity = 0;
			uint32_t nextVertex = 0;
			uint32_t nextIndex = 0;

			std::vector<glm::mat4> modelInstanceData;
			std::vector<uint32_t> tintInstanceData;
			std::vector<uint32_t> texInstanceData;
			std::vector<glm::vec3> albedoInstanceData;
			std::vector<float> metallicInstanceData;
			std::vector<float> roughnessInstanceData;
			std::vector<float> aoInstanceData;

		};

		static std::shared_ptr<InternalData> s_data; //!< Data Internal To Renderer

	};
}
