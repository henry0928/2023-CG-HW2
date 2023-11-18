#include <iostream>
#include "context.h"
#include "program.h"

bool LightProgram::load() {
  long long int pos_offset = 0;
  long long int nor_offset = 12;
  long long int tex_offset = 24;
  long long int element_size = sizeof(float);
  programId = quickCreateProgram(vertProgramFile, fragProgramFIle);
  vector<float> temp_buffer;

  int num_model = (int)ctx->models.size();
  VAO = new GLuint[num_model];
  glGenVertexArrays(num_model, VAO);
  for (int i = 0; i < num_model; i++) {
    glBindVertexArray(VAO[i]);
    Model* model = ctx->models[i];

    GLuint VBO[1];
    glGenBuffers(1, VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, (long long int)32 * model->numVertex, (void*)0, GL_STATIC_DRAW);

    pos_offset = 0;
    nor_offset = 12;
    tex_offset = 24;
    for (int j = 0; j < model->numVertex; j++) {
      glBufferSubData(GL_ARRAY_BUFFER, pos_offset, element_size, &model->positions[3 * j]);
      glBufferSubData(GL_ARRAY_BUFFER, pos_offset + element_size, element_size, &model->positions[3 * j + 1]);
      glBufferSubData(GL_ARRAY_BUFFER, pos_offset + element_size * 2, element_size, &model->positions[3 * j + 2]);
      glBufferSubData(GL_ARRAY_BUFFER, nor_offset, element_size, &model->normals[3 * j]);
      glBufferSubData(GL_ARRAY_BUFFER, nor_offset + element_size, element_size, &model->normals[3 * j + 1]);
      glBufferSubData(GL_ARRAY_BUFFER, nor_offset + element_size * 2, element_size, &model->normals[3 * j + 2]);
      glBufferSubData(GL_ARRAY_BUFFER, tex_offset, element_size, &model->texcoords[2 * j]);
      glBufferSubData(GL_ARRAY_BUFFER, tex_offset + element_size, element_size, &model->texcoords[2 * j + 1]);
      pos_offset = pos_offset + 32;
      nor_offset = nor_offset + 32;
      tex_offset = tex_offset + 32;
    }  // for
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
  }  // for

  /* TODO#4-2: Pass model vertex data to vertex buffer
   *           1. Generate and bind vertex array object (VAO) for each model
   *           2. Generate and bind three vertex buffer objects (VBOs) for each model
   *           3. Pass model positions, normals and texcoords to to VBOs
   * Note:
   *           If you implament BasicProgram properly, You might inherent BasicProgram's load function
   */
  return programId != 0;
}

void LightProgram::doMainLoop() {
  glUseProgram(programId);
  int obj_num = (int)ctx->objects.size();
  for (int i = 0; i < obj_num; i++) {
    int modelIndex = ctx->objects[i]->modelIndex;
    glBindVertexArray(VAO[modelIndex]);

    Model* model = ctx->models[modelIndex];
    const float* p = ctx->camera->getProjectionMatrix();
    GLint pmatLoc = glGetUniformLocation(programId, "Projection");
    glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, p);

    const float* v = ctx->camera->getViewMatrix();
    GLint vmatLoc = glGetUniformLocation(programId, "ViewMatrix");
    glUniformMatrix4fv(vmatLoc, 1, GL_FALSE, v);

    const float* m = glm::value_ptr(ctx->objects[i]->transformMatrix * model->modelMatrix);
    GLint mmatLoc = glGetUniformLocation(programId, "ModelMatrix");
    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, m);

    glm::mat4 TIModelMatrix = glm::transpose(glm::inverse(ctx->objects[i]->transformMatrix * model->modelMatrix));
    GLint TIMLoc = glGetUniformLocation(programId, "TIModelMatrix");
    glUniformMatrix4fv(TIMLoc, 1, GL_FALSE, glm::value_ptr(TIModelMatrix));

    // pass viewpos
    GLint vpLoc = glGetUniformLocation(programId, "viewPos");
    glUniform3fv(vpLoc, 1, ctx->camera->getPosition()); 

    // pass material
    Material _material = ctx->objects[i]->material;
    GLint matamLoc = glGetUniformLocation(programId, "material.ambient");
    glUniform3fv(matamLoc, 1, glm::value_ptr(_material.ambient)); 
    GLint matdiLoc = glGetUniformLocation(programId, "material.diffuse");
    glUniform3fv(matdiLoc, 1, glm::value_ptr(_material.diffuse)); 
    GLint matspLoc = glGetUniformLocation(programId, "material.specular");
    glUniform3fv(matspLoc, 1, glm::value_ptr(_material.specular)); 
    GLint matshLoc = glGetUniformLocation(programId, "material.shininess");
    glUniform1f(matshLoc,  _material.shininess); 

    // pass dl 
    GLint dlenLoc = glGetUniformLocation(programId, "dl.enable");
    glUniform1i(dlenLoc, ctx->directionLightEnable);
    GLint dldiLoc = glGetUniformLocation(programId, "dl.direction");
    glUniform3fv(dldiLoc, 1, glm::value_ptr(ctx->directionLightDirection));
    GLint dlliLoc = glGetUniformLocation(programId, "dl.lightColor");
    glUniform3fv(dlliLoc, 1, glm::value_ptr(ctx->directionLightColor)); 

    //pass pl
    GLint plenLoc = glGetUniformLocation(programId, "pl.enable");
    glUniform1i(plenLoc, ctx->pointLightEnable);
    GLint pldiLoc = glGetUniformLocation(programId, "pl.position");
    glUniform3fv(pldiLoc, 1, glm::value_ptr(ctx->pointLightPosition));
    GLint plliLoc = glGetUniformLocation(programId, "pl.lightColor");
    glUniform3fv(plliLoc, 1, glm::value_ptr(ctx->pointLightColor)); 
    GLint plcoLoc = glGetUniformLocation(programId, "pl.constant");
    glUniform1f(plcoLoc,  ctx->pointLightConstant); 
    GLint pllinLoc = glGetUniformLocation(programId, "pl.linear");
    glUniform1f(pllinLoc,  ctx->pointLightLinear); 
    GLint plquLoc = glGetUniformLocation(programId, "pl.quadratic");
    glUniform1f(plquLoc,  ctx->pointLightQuardratic); 

    // pass sl
    GLint slenLoc = glGetUniformLocation(programId, "sl.enable");
    glUniform1i(slenLoc, ctx->spotLightEnable);
    GLint slpoLoc = glGetUniformLocation(programId, "sl.position");
    glUniform3fv(slpoLoc, 1, glm::value_ptr(ctx->spotLightPosition));
    GLint sldiLoc = glGetUniformLocation(programId, "sl.direction");
    glUniform3fv(sldiLoc, 1, glm::value_ptr(ctx->spotLightDirection));
    GLint slliLoc = glGetUniformLocation(programId, "sl.lightColor");
    glUniform3fv(slliLoc, 1, glm::value_ptr(ctx->spotLightColor)); 
    GLint slcuLoc = glGetUniformLocation(programId, "sl.cutOff");
    glUniform1f(slcuLoc,  ctx->spotLightCutOff); 
    GLint slcoLoc = glGetUniformLocation(programId, "sl.constant");
    glUniform1f(slcoLoc,  ctx->spotLightConstant); 
    GLint sllinLoc = glGetUniformLocation(programId, "sl.linear");
    glUniform1f(sllinLoc,  ctx->spotLightLinear); 
    GLint slquLoc = glGetUniformLocation(programId, "sl.quadratic");
    glUniform1f(slquLoc,  ctx->spotLightQuardratic); 

    glUniform1i(glGetUniformLocation(programId, "ourTexture"), 0);
    glBindTexture(GL_TEXTURE_2D, model->textures[ctx->objects[i]->textureIndex]);
    glDrawArrays(model->drawMode, 0, model->numVertex);
    glBindVertexArray(0);
  } // for

  /* TODO#4-3: Render objects with shader
   *           1. use and bind program (BasicProgram::programId)
   *           2. Iterate all objects (ctx->objects)
   *           3. Load currect model VAO for object
   *           4. Pass projection, view, model matrix to shaders
   *           5. Pass light and material parameters to shader
   *           6. Pass model texture to shaders
   *           7. Draw with glDrawArrays
   * Note:
   *           1. light paramters are providered in context.h
   *           2. material parameter for each object get be found in ctx->objects[i]->material
   */

  glUseProgram(0);
}
