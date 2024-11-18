#include "Scene.h"
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>
#include <cmath>
#include <cstdio>
#include "Game.h"
#include "Core/Shader.h"
#include "Core/Mesh.h"
#include "Core/Model.h"
#include "Constants.h"
#include <random>

// Global Variables
std::unique_ptr<Shader> sceneShader;
std::unique_ptr<Shader> depthShader;
std::unique_ptr<Shader> screenQuadShader;
std::unique_ptr<Shader> bubbleShader;
std::unique_ptr<Shader> ssaoShader;
std::unique_ptr<Shader> ssaoBlurShader;

std::unique_ptr<Mesh> screenQuad;
std::unique_ptr<Model> bubble;
std::unique_ptr<Model> sceneModel;

glm::vec3 bubblePos(0, 0, 0);
unsigned int FBO, depthTexture, normalTexture, ssaoTexture, noiseTexture, ssaoFBO, ssaoBlurFBO; 
std::vector<glm::vec3> kernel;
std::vector<glm::vec3> ssaoNoise;

Scene::Scene()
{
	// Load Shaders
	sceneShader = std::make_unique<Shader>("shaders/scene.vs", "shaders/scene.fs");
	depthShader = std::make_unique<Shader>("shaders/depth.vs", "shaders/depth.fs");
	screenQuadShader = std::make_unique<Shader>("shaders/screenquad.vs", "shaders/screenquad.fs");
	bubbleShader = std::make_unique<Shader>("shaders/bubble.vs", "shaders/bubble.fs");
	ssaoShader = std::make_unique<Shader>("shaders/ssao.vs", "shaders/ssao.fs");
	ssaoBlurShader = std::make_unique<Shader>("shaders/ssaoBlur.vs", "shaders/ssaoBlur.fs");

	// Load models
	bubble = std::make_unique<Model>("assets/sphere.obj");
	sceneModel = std::make_unique<Model>("assets/scene.obj");
	printf("Scene loaded\n");

	// Setup SSAO noise texture

	// Screen Quad for post-processing
	std::vector<Vertex> screenQuadVerts = {
		{{-1, -1, 0}, {0, 0, 0}, {0, 0}},
		{{1, -1, 0}, {0, 0, 0}, {1, 0}},
		{{1, 1, 0}, {0, 0, 0}, {1, 1}},
		{{-1, 1, 0}, {0, 0, 0}, {0, 1}},
	};

	std::vector<Index> screenQuadInds = {
		0, 1, 2,
		2, 3, 0};
	//SSAO Kernel Computation
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

	for (int i = 0; i < 16; ++i)
	{
		glm::vec3 sample(
			distribution(generator),
			distribution(generator),
			distribution(generator));

		sample = glm::normalize(sample);
		sample *= distribution(generator); // Scale the sample by a random value to adjust occlusion
		kernel.push_back(sample);
	}
	//
	screenQuad = std::make_unique<Mesh>(screenQuadVerts, screenQuadInds);

	// Generate Depth and Normal Textures
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glm::vec2 windowSize = Game::GetWindow().GetSize();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, windowSize.x, windowSize.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Setup Framebuffer and Attach Depth Texture
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	// Create the normal texture (G-buffer)
	GLuint normalTexture;
	glGenTextures(1, &normalTexture);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Attach the normal texture to the framebuffer (FBO) in the geometry pass
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GenerateSSAOTexture();
	//
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	glGenTextures(1, &ssaoTexture);
	glBindTexture(GL_TEXTURE_2D, ssaoTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, Game::GetWindow().GetSize().x, Game::GetWindow().GetSize().y, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::GenerateSSAOTexture()
{
	ssaoNoise.resize(16);
	for (unsigned int i = 0; i < 16; ++i)
	{
		ssaoNoise[i] = glm::vec3(
			(rand() % 2000 - 1000) / 1000.0f,
			(rand() % 2000 - 1000) / 1000.0f,
			0.0f);
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

static void RenderSceneDepth(Shader &shader)
{
	shader.Use();
	shader.SetMatrix4f("modelView", Game::GetCamera().GetView());
	shader.SetMatrix4f("projection", Game::GetCamera().GetProjection());
	shader.SetFloat("near", NEAR);
	shader.SetFloat("far", FAR);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	sceneModel->Draw(shader);
}

static void RenderScene(Shader &shader)
{
	shader.Use();
	shader.SetMatrix4f("modelView", Game::GetCamera().GetView());
	shader.SetMatrix4f("projection", Game::GetCamera().GetProjection());

	shader.SetMatrix4f("view", Game::GetCamera().GetView());
	shader.SetVec3f("color", {0.3f, 0.3f, 0.3f});
	shader.SetVec3f("lightDir", {0.5f, -0.72f, -0.65f});

	sceneModel->Draw(shader);
}

void Scene::RenderSSAO(Shader &shader)
{
	shader.Use();
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	shader.SetMatrix4f("projection", Game::GetCamera().GetProjection());
	shader.SetMatrix4f("view", Game::GetCamera().GetView());
	shader.SetVec3f("sampleKernel", glm::value_ptr(kernel[0]), 16);

	// Pass other uniforms (depth, normal)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	//
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	//
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	//

	screenQuad->Draw(shader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::RenderSSAOBlur(Shader &shader)
{
	shader.Use();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	
	screenQuad->Draw(shader);
}

static void RenderBubble(Shader &shader)
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, bubblePos);
	glm::vec3 baseScale(2.0f);
	model = glm::scale(model, baseScale);

	shader.Use();
	shader.SetMatrix4f("modelView", Game::GetCamera().GetView() * model);
	shader.SetMatrix4f("projection", Game::GetCamera().GetProjection());
	shader.SetVec3f("color", {0, 0.9f, 1});
	shader.SetFloat("alpha", 0.01f);
	shader.SetVec2f("screensize", Game::GetWindow().GetSize());
	shader.SetFloat("near", NEAR);
	shader.SetFloat("far", FAR);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	bubble->Draw(shader);
}

void Scene::ApplySSAO(Shader &shader)
{
	shader.Use();
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoTexture);
	sceneModel->Draw(shader);
}

void Scene::Render()
{
	// Step 1: Render the scene to the depth and normal buffers
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	RenderSceneDepth(*depthShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderScene();
	// Step 2: Render the SSAO pass
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	RenderSSAO(*ssaoShader);

	// Step 3: Apply the SSAO blur
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	RenderSSAOBlur(*ssaoBlurShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ApplySSAO(*screenQuadShader);

	glDepthMask(GL_FALSE);
	glCullFace(GL_FRONT);
	RenderBubble(*bubbleShader);

	glCullFace(GL_BACK);
	RenderBubble(*bubbleShader);

	glDepthMask(GL_TRUE);
}
