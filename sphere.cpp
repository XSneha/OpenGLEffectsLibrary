#pragma once0
//#include "headers/Globals.h"


/*************************************************************
 *
 * Creating Sphere
 *
 *************************************************************/
/*void makeSphere(float radius, int slices, int stacks) {

    //Function prototype
    void addTriangle(float[4][3], float[4][3], float[4][2]);

    const float PI = 3.14159265358979323846;
    float drho = PI / (float)stacks;
    float dtheta = 2.0f * PI / (float)slices;
    float ds = 1.0f / (float)slices;
    float dt = 1.0f / (float)stacks;
    float t = 1.0f;
    float s = 0.0f;

    int numIndices = slices * stacks * 6;
    gMaxNumberOfElements = numIndices;

    int iNumIndices = numIndices / 3;

    gpSphereVertices = (float*)malloc(iNumIndices * 3 * sizeof(float));
    if (gpSphereVertices == NULL) {
        fprintf(file, "Memory allocation for gpSphereVertices failed\nExitting now!\n");
        DestroyWindow(ghwnd);
    }

    gpSphereNormals = (float*)malloc(iNumIndices * 3 * sizeof(float));
    if (gpSphereNormals == NULL) {
        fprintf(gErrorFile, "Memory allocation for gpSphereNormals failed\nExitting now!\n");
        DestroyWindow(ghwnd);
    }

    gpSphereTexCoords = (float*)malloc(iNumIndices * 2 * sizeof(float));
    if (gpSphereTexCoords == NULL) {
        fprintf(gErrorFile, "Memory allocation for gpSphereTexCoords failed\nExitting now!\n");
        DestroyWindow(ghwnd);
    }

    gpSphereElements = (short*)malloc(iNumIndices * 3 * sizeof(short));
    if (gpSphereElements == NULL) {
        fprintf(gErrorFile, "Memory allocation for gpSphereElements failed\nExitting now!\n");
        DestroyWindow(ghwnd);
    }

    for (int i = 0; i < stacks; i++) {
        float rho = i * drho;
        float srho = sin(rho);
        float crho = cos(rho);
        float srhodrho = sin(rho + drho);
        float crhodrho = cos(rho + drho);

        float s = 0.0f;

        float vertex[4][3];
        float normal[4][3];
        float texture[4][2];

        for (int j = 0; j < slices; j++) {
            float theta = (j == slices) ? 0.0f : j * dtheta;
            float stheta = -sin(theta);
            float ctheta = cos(theta);

            float x = stheta * srho;
            float y = ctheta * srho;
            float z = crho;

            texture[0][0] = s;
            texture[0][1] = t;
            normal[0][0] = x;
            normal[0][1] = y;
            normal[0][2] = z;
            vertex[0][0] = x * radius;
            vertex[0][1] = y * radius;
            vertex[0][2] = z * radius;

            x = stheta * srhodrho;
            y = ctheta * srhodrho;
            z = crhodrho;

            texture[1][0] = s;
            texture[1][1] = t - dt;
            normal[1][0] = x;
            normal[1][1] = y;
            normal[1][2] = z;
            vertex[1][0] = x * radius;
            vertex[1][1] = y * radius;
            vertex[1][2] = z * radius;

            theta = ((j + 1) == slices) ? 0.0f : (j + 1) * dtheta;
            stheta = -sin(theta);
            ctheta = cos(theta);

            x = stheta * srho;
            y = ctheta * srho;
            z = crho;

            s += ds;
            texture[2][0] = s;
            texture[2][1] = t;
            normal[2][0] = x;
            normal[2][1] = y;
            normal[2][2] = z;
            vertex[2][0] = x * radius;
            vertex[2][1] = y * radius;
            vertex[2][2] = z * radius;

            x = stheta * srhodrho;
            y = ctheta * srhodrho;
            z = crhodrho;

            texture[3][0] = s;
            texture[3][1] = t - dt;
            normal[3][0] = x;
            normal[3][1] = y;
            normal[3][2] = z;
            vertex[3][0] = x * radius;
            vertex[3][1] = y * radius;
            vertex[3][2] = z * radius;

            addTriangle(vertex, normal, texture);

            vertex[0][0] = vertex[1][0];
            vertex[0][1] = vertex[1][1];
            vertex[0][2] = vertex[1][2];
            normal[0][0] = normal[1][0];
            normal[0][1] = normal[1][1];
            normal[0][2] = normal[1][2];
            texture[0][0] = texture[1][0];
            texture[0][1] = texture[1][1];

            vertex[1][0] = vertex[3][0];
            vertex[1][1] = vertex[3][1];
            vertex[1][2] = vertex[3][2];
            normal[1][0] = normal[3][0];
            normal[1][1] = normal[3][1];
            normal[1][2] = normal[3][2];
            texture[1][0] = texture[3][0];
            texture[1][1] = texture[3][1];

            addTriangle(vertex, normal, texture);
        }
        t -= dt;
    }
    glGenVertexArrays(1, &gVao_Sphere);
    glBindVertexArray(gVao_Sphere);

    //Position
    glGenBuffers(1, &gVbo_SpherePosition);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_SpherePosition);
    glBufferData(GL_ARRAY_BUFFER, iNumIndices * 3 * sizeof(float), gpSphereVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(DRG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(DRG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Normal
    glGenBuffers(1, &gVbo_SphereNormals);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_SphereNormals);
    glBufferData(GL_ARRAY_BUFFER, iNumIndices * 3 * sizeof(float), gpSphereNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(DRG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(DRG_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Texture
    glGenBuffers(1, &gVbo_SphereTexture);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_SphereTexture);
    glBufferData(GL_ARRAY_BUFFER, iNumIndices * 2 * sizeof(float), gpSphereTexCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(DRG_ATTRIBUTE_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(DRG_ATTRIBUTE_TEXTURE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Elements
    glGenBuffers(1, &gVbo_SphereElements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_SphereElements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iNumIndices * 3 * sizeof(short), gpSphereElements, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (gpSphereVertices) {
        free(gpSphereVertices);
        gpSphereVertices = NULL;
    }
    if (gpSphereNormals) {
        free(gpSphereNormals);
        gpSphereNormals = NULL;
    }
    if (gpSphereTexCoords) {
        free(gpSphereTexCoords);
        gpSphereTexCoords = NULL;
    }
    if (gpSphereElements) {
        free(gpSphereElements);
        gpSphereElements = NULL;
    }
}

void addTriangle(float vertices[4][3], float normals[4][3], float textures[4][2]) {

    //Function prototype
    bool isFoundIdentical(float, float, float);

    //Code
    const float diff = 0.00001f;
    int i, j;

    for (i = 0; i < 3; i++) {
        float squaredVectorLength = (normals[i][0] * normals[i][0]) + (normals[i][1] * normals[i][1]) + (normals[i][2] * normals[i][2]);
        float squaredRootOfSquareVectorLength = sqrt(squaredVectorLength);

        normals[i][0] = normals[i][0] / squaredRootOfSquareVectorLength;
        normals[i][1] = normals[i][1] / squaredRootOfSquareVectorLength;
        normals[i][2] = normals[i][2] / squaredRootOfSquareVectorLength;
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < gNumberOfVertices; j++) {
            if (isFoundIdentical(gpSphereVertices[j * 3], vertices[i][0], diff) &&
                isFoundIdentical(gpSphereVertices[j * 3 + 1], vertices[i][1], diff) &&
                isFoundIdentical(gpSphereVertices[j * 3 + 2], vertices[i][2], diff) &&
                isFoundIdentical(gpSphereNormals[j * 3], normals[i][0], diff) &&
                isFoundIdentical(gpSphereNormals[j * 3 + 1], normals[i][1], diff) &&
                isFoundIdentical(gpSphereNormals[j * 3 + 2], normals[i][2], diff) &&
                isFoundIdentical(gpSphereTexCoords[j * 2], textures[i][0], diff) &&
                isFoundIdentical(gpSphereTexCoords[j * 2 + 1], textures[i][1], diff)) {
                gpSphereElements[gNumberOfElements] = j;
                gNumberOfElements++;
                break;
            }
        }

        if (j == gNumberOfVertices && gNumberOfVertices < gMaxNumberOfElements && gNumberOfElements < gMaxNumberOfElements) {
            gpSphereVertices[gNumberOfVertices * 3] = vertices[i][0];
            gpSphereVertices[gNumberOfVertices * 3 + 1] = vertices[i][1];
            gpSphereVertices[gNumberOfVertices * 3 + 2] = vertices[i][2];

            gpSphereNormals[gNumberOfVertices * 3] = normals[i][0];
            gpSphereNormals[gNumberOfVertices * 3 + 1] = normals[i][1];
            gpSphereNormals[gNumberOfVertices * 3 + 2] = normals[i][2];

            gpSphereTexCoords[gNumberOfVertices * 2] = textures[i][0];
            gpSphereTexCoords[gNumberOfVertices * 2 + 1] = textures[i][1];

            gpSphereElements[gNumberOfElements] = gNumberOfVertices;

            gNumberOfElements++;
            gNumberOfVertices++;
        }
    }
}

bool isFoundIdentical(float val1, float val2, float diff) {

    //Code
    if (fabs(val1 - val2) < diff)
        return true;
    else
        return false;
}
*/
