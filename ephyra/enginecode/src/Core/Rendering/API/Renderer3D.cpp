/** \file renderer3D.cpp */

#include "Ephyra_pch.h"

#include "Core/Initialization/GlobalProperties.h"
#include "Core/Rendering/Renderer/Renderer3D.h"

#include <Glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <numeric>
#include <algorithm>


namespace Engine
{
	vertexBufferLayout Renderer3DVertex::s_layout = vertexBufferLayout( { ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::Float2, ShaderDataType::Float4, ShaderDataType::Float4 } );

	std::shared_ptr<Renderer3D::InternalData> Renderer3D::s_data = nullptr;

	void Renderer3D::init(uint32_t vertexCapacity, uint32_t indexCapacity, uint32_t batchSize)
	{
		
		if (!RendererCommon::defaultTexture)
		{
			unsigned char whitePx[4] = { 255, 255, 255, 255 };
			RendererCommon::defaultTexture.reset(Texture::create(1, 1, 4, whitePx));

			RendererCommon::defaultSubTexture = SubTexture(RendererCommon::defaultTexture, { 0.f,0.f }, { 1.f, 1.f });
		}

		if (!RendererCommon::m_textUM)
		{
			RendererCommon::m_textUM.reset(new TextureUnitManager(32));
		}

		if (!RendererCommon::textureUnits)
		{
			RendererCommon::textureUnits.reset(new std::array<int32_t, 32>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 });
		}

		if (!RendererCommon::colorFBOTexture)
		{
			RendererCommon::colorFBOTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		if (!RendererCommon::depthFBOTexture)
		{
			RendererCommon::depthFBOTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 0, nullptr));
		}

		if (!RendererCommon::colorFBO)
		{	
			RendererCommon::colorFBO = FrameBuffer::create();
			RendererCommon::colorFBO->bind();
			RendererCommon::colorFBO->attachColorTexture(RendererCommon::colorFBOTexture->getID(), 0);
			RendererCommon::colorFBO->attachDepthTexture(RendererCommon::depthFBOTexture->getID());
			RendererCommon::colorFBO->unbind();
		}

		s_data.reset(new InternalData);

		s_data->batchCapacity = batchSize;
		s_data->vertexCapacity = vertexCapacity;
		s_data->indexCapacity = indexCapacity;

		s_data->batchQueue.reserve(batchSize);
		s_data->modelInstanceData.reserve(batchSize);
		s_data->tintInstanceData.reserve(batchSize);
		s_data->batchCommands.reserve(batchSize);

		s_data->VAO.reset(VertexArray::create());

		std::shared_ptr<VertexBuffer> VBO_Verts;
		std::shared_ptr<IndexBuffer> IBO;

		VBO_Verts.reset(VertexBuffer::create(nullptr, sizeof(Renderer3DVertex) * vertexCapacity, Renderer3DVertex::s_layout));
		IBO.reset(IndexBuffer::create(nullptr, indexCapacity));

		s_data->VAO->addVertexBuffer(VBO_Verts);
		s_data->VAO->setIndexBuffer(IBO);

		vertexBufferLayout modelLayout = { {ShaderDataType::Mat4, 1, false} };
		std::shared_ptr<VertexBuffer> VBO_Models;
		VBO_Models.reset(VertexBuffer::create(nullptr, batchSize * sizeof(glm::mat4), modelLayout));
		s_data->VAO->addVertexBuffer(VBO_Models);

		vertexBufferLayout tintLayout = { {ShaderDataType::Byte4, 1, true} };
		std::shared_ptr<VertexBuffer> VBO_Tints;
		VBO_Tints.reset(VertexBuffer::create(nullptr, batchSize * sizeof(BYTE) * 4, tintLayout));
		s_data->VAO->addVertexBuffer(VBO_Tints);

		vertexBufferLayout albedoLayout = { {ShaderDataType::FlatInt, 1, false} };
		std::shared_ptr<VertexBuffer> VBO_Albedo;
		VBO_Albedo.reset(VertexBuffer::create(nullptr, batchSize * sizeof(uint32_t), albedoLayout));
		s_data->VAO->addVertexBuffer(VBO_Albedo);

		vertexBufferLayout metallicLayout = { {ShaderDataType::FlatInt, 1, false} };
		std::shared_ptr<VertexBuffer> VBO_Metallic;
		VBO_Metallic.reset(VertexBuffer::create(nullptr, batchSize * sizeof(uint32_t), metallicLayout));
		s_data->VAO->addVertexBuffer(VBO_Metallic);

		vertexBufferLayout roughnessLayout = { {ShaderDataType::FlatInt, 1, false} };
		std::shared_ptr<VertexBuffer> VBO_Roughness;
		VBO_Roughness.reset(VertexBuffer::create(nullptr, batchSize * sizeof(uint32_t), roughnessLayout));
		s_data->VAO->addVertexBuffer(VBO_Roughness);

		vertexBufferLayout aoLayout = { {ShaderDataType::FlatInt, 1, false} };
		std::shared_ptr<VertexBuffer> VBO_Ao;
		VBO_Ao.reset(VertexBuffer::create(nullptr, batchSize * sizeof(uint32_t), aoLayout));
		s_data->VAO->addVertexBuffer(VBO_Ao);

		vertexBufferLayout normalLayout = { {ShaderDataType::FlatInt, 1, false} };
		std::shared_ptr<VertexBuffer> VBO_Normal;
		VBO_Normal.reset(VertexBuffer::create(nullptr, batchSize * sizeof(uint32_t), normalLayout));
		s_data->VAO->addVertexBuffer(VBO_Normal);

		s_data->commands.reset(IndirectBuffer::create(nullptr, batchSize));

		s_data->cameraUBO.reset(UniformBuffer::create(uniformBufferLayout({
			{"u_projection", ShaderDataType::Mat4},
			{"u_view", ShaderDataType::Mat4}
			})));

		s_data->lightsUBO.reset(UniformBuffer::create(uniformBufferLayout({
			{"u_viewPos", ShaderDataType::Float3},
			})));

		Renderer2D::init();

		RendererCommon::postProcessor = PostProcessing::create();

	}

	void Renderer3D::begin(const SceneWideUniforms& sceneWideUniforms)
	{

		RendererCommon::colorFBO->bind();
		
		RendererCommon::colorFBO->clear();
		glViewport(SCR_POSX, SCR_POSY, SCR_WIDTH, SCR_HEIGHT);
		s_data->cameraUBO->bindUniformBuffer();
		s_data->cameraUBO->uploadData("u_projection", sceneWideUniforms.at("u_projection").second);
		s_data->cameraUBO->uploadData("u_view", sceneWideUniforms.at("u_view").second);

		s_data->lightsUBO->bindUniformBuffer();
		s_data->lightsUBO->uploadData("u_viewPos", sceneWideUniforms.at("u_viewPos").second);
	}

	void Renderer3D::submit(const Geometry& geometry, const std::shared_ptr<Material>& material, const glm::mat4& model)
	{

		if (material->isFlagSet(Material::flag_batched))
		{
			if (s_data->batchQueue.size() == s_data->batchCapacity) flushBatch();

			s_data->batchQueue.push_back({ geometry, material, model});
		}
		else
		{
			// Bind Shader
			auto& shader = material->getShader();

			shader->useShader(s_data->VAO->getRenderID());

			RendererCommon::colorFBO->bind();

			uint32_t texUnit[5];

			std::vector<std::shared_ptr<Texture>> textures = material->getTextures();
			for (int i = 0; i < textures.size(); i++)
			{
				RendererCommon::m_textUM->getUnit(textures[i]->getID(), texUnit[i]);
				textures[i]->load(texUnit[i]);
			}

			shader->uploadIntArray("u_texData", RendererCommon::textureUnits->data(), 32);

			if (!RendererCommon::lightPos.empty())
				shader->uploadFloat3Array("u_lightPos", RendererCommon::lightPos.data(), 64);
			else
			{
				std::vector<glm::vec3> temp;
				for (int i = 0; i < 16; i++)
					temp.push_back({ 0.f, 0.f, 0.f });
				shader->uploadFloat3Array("u_lightPos", temp.data(), 4);
			}

			if (!RendererCommon::lightColour.empty())
				shader->uploadFloat3Array("u_lightColour", RendererCommon::lightColour.data(), 64);
			else
			{
				std::vector<glm::vec3> temp;
				for (int i = 0; i < 16; i++)
					temp.push_back({ 0.f, 0.f, 0.f });
				shader->uploadFloat3Array("u_lightColour", temp.data(), 64);
			}

			shader->uploadInt("ImmediateMode", 1);

			shader->uploadInt("AlbedoTex", texUnit[0]);
			shader->uploadInt("RoughnessTex", texUnit[1]);
			shader->uploadInt("MetallicTex", texUnit[2]);
			shader->uploadInt("AOTex", texUnit[3]);
			shader->uploadInt("NormalTex", texUnit[4]);

			// Apply Material Uniforms (per draw uniforms)
			shader->uploadMat4("ModelMat", model);

			shader->uploadFloat4("TintCol", material->getTint());

			s_data->VAO->bindIndexBuffer();

			// Submit the draw call
			glDrawElementsBaseVertex(GL_TRIANGLES, geometry.indexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * geometry.firstIndex), geometry.firstVertex);
		}

	}

	void Renderer3D::flush()
	{
		RendererCommon::colorFBO->bind();
		if (s_data->batchQueue.size() > 0) flushBatch();
	}

	void Renderer3D::end()
	{
		flush();

		//RendererCommon::colorFBO->unbind();

		//RendererCommon::colorFBO->clear();
	}

	void Renderer3D::end(bool enabledEffects[16])
	{
		flush();

		RendererCommon::frameCount++;

		uint32_t texID = RendererCommon::colorFBOTexture->getID();
		if (enabledEffects[0])
			texID = RendererCommon::postProcessor->ApplyDOFEffect(RendererCommon::depthFBOTexture->getID(), texID);
		if (enabledEffects[1])
			texID = RendererCommon::postProcessor->ApplyVolumetricEffect(RendererCommon::depthFBOTexture->getID(), texID);
		if (enabledEffects[2])
			texID = RendererCommon::postProcessor->ApplyBloomEffect(texID);
		if (enabledEffects[3])
			texID = RendererCommon::postProcessor->ApplyToneMappingEffect(texID);
		if (enabledEffects[4])
			texID = RendererCommon::postProcessor->ApplyVignetteEffect(texID);
		if (enabledEffects[5])
			texID = RendererCommon::postProcessor->ApplyRetroGradeEffect(texID, RendererCommon::frameCount);

		RendererCommon::postProcessor->updateColorFBO(texID, RendererCommon::colorFBOTexture->getID());
	}

	void Renderer3D::initShader(std::shared_ptr<Shader> shader)
	{
		s_data->cameraUBO->attachShaderBlock(shader, "b_camera");
		s_data->lightsUBO->attachShaderBlock(shader, "b_lights");
	}

	bool Renderer3D::addGeometry(std::vector<Renderer3DVertex> vertices, std::vector<uint32_t> indices, Geometry& geo)
	{
		uint32_t vertexCount = vertices.size();
		uint32_t indexCount = indices.size();

		if (vertexCount + s_data->nextVertex > s_data->vertexCapacity) return false;
		if (indexCount + s_data->nextIndex > s_data->indexCapacity) return false;

		auto VBO = s_data->VAO->getVertexBuffer().at(0);
		auto IBO = s_data->VAO->getIndexBuffer();

		VBO->edit(vertices.data(), vertexCount * sizeof(Renderer3DVertex), s_data->nextVertex * sizeof(Renderer3DVertex));
		IBO->edit(indices.data(), indexCount, s_data->nextIndex);

		geo.id = s_data->batchCommands.size();
		geo.firstVertex = s_data->nextVertex;
		geo.firstIndex = s_data->nextIndex;
		geo.vertexCount = vertexCount;
		geo.indexCount = indexCount;

		s_data->nextVertex += vertexCount;
		s_data->nextIndex += indexCount;

		s_data->batchCommands.push_back({ 0,0,0,0,0 });
		return true;

	}

	void Renderer3D::flushBatch()
	{
		//Sort Batch Queue by Shader then By geometryID
		std::sort(s_data->batchQueue.begin(), s_data->batchQueue.end(),
			[](BatchQueueEntry& a, BatchQueueEntry& b) {
				if (a.material->getShader() == b.material->getShader())
				{
					return a.geometry.id < b.geometry.id;
				}
				return a.material->getShader() < b.material->getShader();
			}
		);

		uint32_t runningInstanceCount = 0;
		uint32_t texUnit[5];

		for (auto& bqe : s_data->batchQueue)
		{
			// Not Using Vertex Count of Geometry
			auto& index = bqe.geometry.id;

			if (s_data->batchCommands.at(index).drawCount == 0)
			{
				s_data->batchCommands.at(index).firstVertex = bqe.geometry.firstVertex;
				s_data->batchCommands.at(index).firstIndex = bqe.geometry.firstIndex;
				s_data->batchCommands.at(index).drawCount = bqe.geometry.indexCount;
				s_data->batchCommands.at(index).firstInstance = runningInstanceCount;
			}

			// Increment Instance Counts
			s_data->batchCommands.at(index).instanceCount++;
			runningInstanceCount++;

			// Add Instanced Variables
			s_data->modelInstanceData.push_back(bqe.model);
			if(bqe.material->isFlagSet(Material::flag_tint))
				s_data->tintInstanceData.push_back(RendererCommon::pack(bqe.material->getTint()));
			else
				s_data->tintInstanceData.push_back(RendererCommon::pack(glm::vec4(1.f)));

			if (RendererCommon::m_textUM->isFull())
			{
				flushBatchCommands(bqe.material->getShader(), runningInstanceCount);
				runningInstanceCount = 0;
			}

			std::vector<std::shared_ptr<Texture>> textures = bqe.material->getTextures();
			for (int i = 0; i < textures.size(); i++)
			{
				RendererCommon::m_textUM->getUnit(textures[i]->getID(), texUnit[i]);
				textures[i]->load(texUnit[i]);
			}

			s_data->albedoInstanceData.push_back(texUnit[0]);
			s_data->metallicInstanceData.push_back(texUnit[1]);
			s_data->roughnessInstanceData.push_back(texUnit[2]);
			s_data->aoInstanceData.push_back(texUnit[3]);
			s_data->normalInstanceData.push_back(texUnit[4]);
		}

		if (runningInstanceCount > 0)
		{
			flushBatchCommands(s_data->batchQueue.back().material->getShader(), runningInstanceCount);
			runningInstanceCount = 0;
		}

		s_data->batchQueue.clear();

		s_data->modelInstanceData.clear();
		s_data->tintInstanceData.clear();

		s_data->albedoInstanceData.clear();
		s_data->metallicInstanceData.clear();
		s_data->roughnessInstanceData.clear();
		s_data->aoInstanceData.clear();
		s_data->normalInstanceData.clear();

		for (auto& command : s_data->batchCommands)
		{
			command.instanceCount = 0;
			command.drawCount = 0;
		}
	}

	void Renderer3D::flushBatchCommands(std::shared_ptr<Shader>& shader, uint32_t instanceCount)
	{
		// Use Shader
		shader->useShader(s_data->VAO->getRenderID());

		// Upload Tex Units
		shader->uploadInt("ImmediateMode", 0);

		shader->uploadIntArray("u_texData", RendererCommon::textureUnits->data(), 32);

		if (!RendererCommon::lightPos.empty())
			shader->uploadFloat3Array("u_lightPos", RendererCommon::lightPos.data(), 64);
		else
		{
			std::vector<glm::vec3> temp;
			for (int i = 0; i < 16; i ++)
				temp.push_back({ 0.f, 0.f, 0.f });
			shader->uploadFloat3Array("u_lightPos", temp.data(), 4);
		}

		if (!RendererCommon::lightColour.empty())
			shader->uploadFloat3Array("u_lightColour", RendererCommon::lightColour.data(), 64);
		else
		{
			std::vector<glm::vec3> temp;
			for (int i = 0; i < 16; i++)
				temp.push_back({ 0.f, 0.f, 0.f });
			shader->uploadFloat3Array("u_lightColour", temp.data(), 64);
		}

		// Upload Instance Data
		auto VBO_Models = s_data->VAO->getVertexBuffer().at(1);
		auto VBO_Tints = s_data->VAO->getVertexBuffer().at(2);

		auto VBO_Albedo = s_data->VAO->getVertexBuffer().at(3);
		auto VBO_Metallic = s_data->VAO->getVertexBuffer().at(4);
		auto VBO_Roughness = s_data->VAO->getVertexBuffer().at(5);
		auto VBO_AO = s_data->VAO->getVertexBuffer().at(6);
		auto VBO_Normal = s_data->VAO->getVertexBuffer().at(7);

		VBO_Models->edit(s_data->modelInstanceData.data(), sizeof(glm::mat4) * instanceCount, 0);
		VBO_Tints->edit(s_data->tintInstanceData.data(), sizeof(uint32_t) * instanceCount, 0);

		VBO_Albedo->edit(s_data->albedoInstanceData.data(), sizeof(uint32_t) * instanceCount, 0);
		VBO_Metallic->edit(s_data->metallicInstanceData.data(), sizeof(uint32_t) * instanceCount, 0);
		VBO_Roughness->edit(s_data->roughnessInstanceData.data(), sizeof(uint32_t) * instanceCount, 0);
		VBO_AO->edit(s_data->aoInstanceData.data(), sizeof(uint32_t) * instanceCount, 0);
		VBO_Normal->edit(s_data->normalInstanceData.data(), sizeof(uint32_t) * instanceCount, 0);

		s_data->VAO->bindIndexBuffer();

		s_data->commands->edit(s_data->batchCommands.data(), s_data->batchCommands.size(), 0);

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, s_data->batchCommands.size(), 0);
	}

}