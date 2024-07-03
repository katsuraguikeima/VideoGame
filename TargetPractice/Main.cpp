//------- Ignore this ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------

#include"Model.h"
#include<SFML/Audio.hpp>

const unsigned int width = 900;
const unsigned int height = 700;



float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};



int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Configure GLFW so that the window is not resizable
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "Target practice", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 900, y = 700
	glViewport(0, 0, width, height);

	// Generates Shader objects
	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	Shader multipleShader("objectInstancing.vert", "default.frag");

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	multipleShader.Activate();
	glUniform4f(glGetUniformLocation(multipleShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(multipleShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 4.0f, 0.0f));


	std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	std::string tree_object = "/TargetPractice/Resources/models/tree/scene.gltf";
	std::string earth_object = "/TargetPractice/Resources/models/map/scene.gltf";
	std::string target_object = "/TargetPractice/Resources/models/target/scene.gltf";
	std::string steve_object = "/TargetPractice/Resources/models/steve/scene.gltf";
	std::string weapon_pov_object = "/TargetPractice/Resources/models/weapon/scene.gltf";

	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile((parentDir + "/TargetPractice/Resources/sounds/doom.ogg").c_str())) {
		std::cerr << "Error al cargar el archivo de sonido" << std::endl;
		return -1;
	}

	// Crea un objeto de sonido y configura el buffer
	sf::Sound sound;
	sound.setBuffer(buffer);

	// Reproduce el sonido
	sound.setLoop(true);

	sound.play();

	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// All the faces of the cubemap (make sure they are in this exact order)
	std::string facesCubemap[6] =
	{
		 parentDir+"/TargetPractice/Resources/skybox/right.png",
		 parentDir + "/TargetPractice/Resources/skybox/left.png",
		 parentDir + "/TargetPractice/Resources/skybox/top.png",
		 parentDir + "/TargetPractice/Resources/skybox/bottom.png",
		 parentDir + "/TargetPractice/Resources/skybox/front.png",
		 parentDir + "/TargetPractice/Resources/skybox/back.png"
	};

	// Creates the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			if (nrChannels == 4) {
				nrChannels = GL_RGBA;
			}
			else if (nrChannels == 3) {
				nrChannels = GL_RGB;
			}
			else if (nrChannels == 2) {
				nrChannels = GL_RG;
			}
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				nrChannels,
				width,
				height,
				0,
				nrChannels,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}


	//Positions for the earth cube 
	std::vector<glm::vec3> earthCubePositions; 

	//Fill the positions
	for (float i = -52.0f; i <= 52.0f; i += 2.0f) {
		for (float j = -52.0f; j <= 52.0f; j += 2.0f) {
			//Add positions on the edge
			earthCubePositions.push_back(glm::vec3(j, 0.0f, i));
		}
	}

	const unsigned int earthCubeNumber = earthCubePositions.size();
	std::vector <glm::mat4> earthCubeInstanceMatrix;
	for (size_t i = 0; i < earthCubeNumber; i += 1) {
		glm::quat rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rot = glm::mat4_cast(rotation);
		glm::mat4 sca = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), earthCubePositions[i]);

		earthCubeInstanceMatrix.push_back(trans * rot * sca);
	}


	//Matrices for the tree
	std::vector<glm::vec3> treePositions; 

	// Llenar las posiciones
	for (float i = -50.0f; i <= 50.0f; i += 5.0f) {
		for (float j = -50.0f; j <= 50.0f; j += 5.0f) {
			//Add positions on the edge
			if ((i == -50.0f || i == 50.0f || j == -50.0f || j == 50.0f)) {
				treePositions.push_back(glm::vec3(j, 0.0f, i));
			}
		}
	}

	const unsigned int treeNumber = treePositions.size();
	std::vector <glm::mat4> treeInstanceMatrix;
	for (size_t i = 0; i < treeNumber; i += 1) {
		glm::quat rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rot = glm::mat4_cast(rotation);
		glm::mat4 sca = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), treePositions[i]);

		treeInstanceMatrix.push_back(trans * rot * sca);
	}


	//Matrices for the target
	std::vector<glm::vec3> targetPositions{
		glm::vec3(30.0f,1.7f,-30.0f),
		glm::vec3(20.0f,1.7f,-30.0f),
		glm::vec3(10.0f,1.7f,-30.0f),
		glm::vec3(0.0f,1.7f,-30.0f),
		glm::vec3(-10.0f,1.7f,-30.0f),
		glm::vec3(-20.0f,1.7f,-30.0f),
		glm::vec3(-30.0f,1.7f,-30.0f)
	};



	const unsigned int targetNumber = targetPositions.size();
	std::vector <glm::mat4> targetInstanceMatrix;
	for (size_t i = 0; i < targetNumber; i += 1) {
		glm::quat rotation = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rot = glm::mat4_cast(rotation);
		glm::mat4 sca = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), targetPositions[i]);

		targetInstanceMatrix.push_back(trans * rot * sca);
	}


	//Matrices for the steve
	std::vector<glm::vec3> stevePositions{
		glm::vec3(300.0f,  3.0f,  30.0f),
		glm::vec3(20.0f,  3.0f,  30.0f),
		glm::vec3(10.0f,  3.0f,  30.0f),
		glm::vec3(0.0f,  3.0f,  30.0f),
		glm::vec3(-10.0f,  3.0f,  30.0f),
		glm::vec3(-20.0f,  3.0f,  30.0f),
		glm::vec3(-30.0f,  3.0f,  30.0f)
	};

	const unsigned int steveNumber = stevePositions.size();
	std::vector <glm::mat4> steveInstanceMatrix;
	for (size_t i = 0; i < steveNumber; i += 1) {
		glm::mat4 rot = glm::mat4(1.0f);
		rot = glm::rotate(rot, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		rot = glm::rotate(rot, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		rot = glm::rotate(rot, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 sca = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), stevePositions[i]);

		steveInstanceMatrix.push_back(trans * rot * sca);
	}
	// Load in models
	Model earthCube((parentDir + earth_object).c_str(), earthCubeNumber, earthCubeInstanceMatrix);
	Model tree((parentDir + tree_object).c_str(), treeNumber, treeInstanceMatrix);
	Model target((parentDir + target_object).c_str(), targetNumber, targetInstanceMatrix);
	Model steve((parentDir + steve_object).c_str(), steveNumber, steveInstanceMatrix);
	Model pov((parentDir + weapon_pov_object).c_str());

	//Transform matrix for pov object
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 sca = glm::vec3(0.3f, 0.3f, 0.3f);
	glm::vec3 trans = glm::vec3(camera.Position.x, 0.21f, camera.Position.z);
	float angle = 0.0f;
	// Main while loop
	while (!glfwWindowShouldClose(window))
	{

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs (delete this if you have disabled VSync)
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 200.0f);


		multipleShader.Activate();
		tree.Draw(multipleShader, camera);
		earthCube.Draw(multipleShader, camera);
		target.Draw(multipleShader, camera);
		steve.Draw(multipleShader, camera);
		shaderProgram.Activate();
		trans = camera.Position / 16.34f;

		trans.y = 0.21f;
		rot = glm::rotate(rot, glm::radians(camera.Orientation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		pov.Draw(shaderProgram, camera, trans, rot, sca);

		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
		// The last row and column affect the translation of the skybox (which we don't want to affect)
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		// where an object is present (a depth of 1.0f will always fail against any object's depth value)
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	shaderProgram.Delete();
	skyboxShader.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}