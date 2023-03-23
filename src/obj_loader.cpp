// OBJ_Loader.h - A Single Header OBJ Model Loader

#include <obj_loader.h>

        // Default Constructor
newMesh::newMesh()
{
}

// Variable Set Constructor
newMesh::newMesh(std::vector<Vertex>& _Vertices, std::vector<unsigned int>& _Indices)
{
    Vertices = _Vertices;
    Indices = _Indices;
}

// glm::vec3 Cross Product
glm::vec3 CrossV3(const glm::vec3 a, const glm::vec3 b)
{
    return glm::vec3(a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

// glm::vec3 Magnitude Calculation
float MagnitudeV3(const glm::vec3 in)
{
    return (sqrtf(powf(in.x, 2) + powf(in.y, 2) + powf(in.z, 2)));
}

// glm::vec3 DotProduct
float DotV3(const glm::vec3 a, const glm::vec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

// Angle between 2 glm::vec3 Objects
float AngleBetweenV3(const glm::vec3 a, const glm::vec3 b)
{
    float angle = DotV3(a, b);
    angle /= (MagnitudeV3(a) * MagnitudeV3(b));
    return angle = acosf(angle);
}

// Projection Calculation of a onto b
glm::vec3 ProjV3(const glm::vec3 a, const glm::vec3 b)
{
    glm::vec3 bn = b / MagnitudeV3(b);
    return bn * DotV3(a, bn);
}

// glm::vec3 Multiplication Opertor Overload
glm::vec3 operator*(const float& left, const glm::vec3& right)
{
    return glm::vec3(right.x * left, right.y * left, right.z * left);
}

// A test to see if P1 is on the same side as P2 of a line segment ab
bool SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b)
{
    glm::vec3 cp1 = CrossV3(b - a, p1 - a);
    glm::vec3 cp2 = CrossV3(b - a, p2 - a);

    if (DotV3(cp1, cp2) >= 0)
        return true;
    else
        return false;
}

// Generate a cross product normal for a triangle
glm::vec3 GenTriNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
{
    glm::vec3 u = t2 - t1;
    glm::vec3 v = t3 - t1;

    glm::vec3 normal = CrossV3(u, v);

    return normal;
}

// Check to see if a glm::vec3 Point is within a 3 glm::vec3 Triangle
bool inTriangle(glm::vec3 point, glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3)
{
    // Test to see if it is within an infinite prism that the triangle outlines.
    bool within_tri_prisim = SameSide(point, tri1, tri2, tri3) && SameSide(point, tri2, tri1, tri3)
        && SameSide(point, tri3, tri1, tri2);

    // If it isn't it will never be on the triangle
    if (!within_tri_prisim)
        return false;

    // Calulate Triangle's Normal
    glm::vec3 n = GenTriNormal(tri1, tri2, tri3);

    // Project the point onto this normal
    glm::vec3 proj = ProjV3(point, n);

    // If the distance from the triangle to the point is 0
    //	it lies on the triangle
    if (MagnitudeV3(proj) == 0)
        return true;
    else
        return false;
}

// Split a String into a string array at a given token
void split(const std::string& in, std::vector<std::string>& out, std::string token)
{
    out.clear();

    std::string temp;

    for (int i = 0; i < int(in.size()); i++)
    {
        std::string test = in.substr(i, token.size());

        if (test == token)
        {
            if (!temp.empty())
            {
                out.push_back(temp);
                temp.clear();
                i += (int)token.size() - 1;
            }
            else
            {
                out.push_back("");
            }
        }
        else if (i + token.size() >= in.size())
        {
            temp += in.substr(i, token.size());
            out.push_back(temp);
            break;
        }
        else
        {
            temp += in[i];
        }
    }
}

// Get tail of string after first token and possibly following spaces
std::string tail(const std::string& in)
{
    size_t token_start = in.find_first_not_of(" \t");
    size_t space_start = in.find_first_of(" \t", token_start);
    size_t tail_start = in.find_first_not_of(" \t", space_start);
    size_t tail_end = in.find_last_not_of(" \t");
    if (tail_start != std::string::npos && tail_end != std::string::npos)
    {
        return in.substr(tail_start, tail_end - tail_start + 1);
    }
    else if (tail_start != std::string::npos)
    {
        return in.substr(tail_start);
    }
    return "";
}

// Get first token of string
inline std::string firstToken(const std::string& in)
{
    if (!in.empty())
    {
        size_t token_start = in.find_first_not_of(" \t");
        size_t token_end = in.find_first_of(" \t", token_start);
        if (token_start != std::string::npos && token_end != std::string::npos)
        {
            return in.substr(token_start, token_end - token_start);
        }
        else if (token_start != std::string::npos)
        {
            return in.substr(token_start);
        }
    }
    return "";
}

// Get element at given index position
template <class T>
inline const T& getElement(const std::vector<T>& elements, std::string& index)
{
    int idx = std::stoi(index);
    if (idx < 0)
        idx = int(elements.size()) + idx;
    else
        idx--;
    return elements[idx];
}

// Default Constructor
Loader::Loader()
{
}
Loader::~Loader()
{
    LoadednewMeshes.clear();
}

// Load a file into the loader
//
// If file is loaded return true
//
// If the file is unable to be found
// or unable to be loaded return false
bool Loader::LoadFile(std::string Path)
{
    // If the file is not an .obj file return false
    if (Path.substr(Path.size() - 4, 4) != ".obj")
        return false;

    std::ifstream file(Path);

    if (!file.is_open())
        return false;

    LoadednewMeshes.clear();
    LoadedVertices.clear();
    LoadedIndices.clear();

    std::vector<glm::vec3> Positions;
    std::vector<glm::vec2> TCoords;
    std::vector<glm::vec3> Normals;

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    std::vector<std::string> newMeshMatNames;

    bool listening = false;
    std::string newMeshname;

    newMesh tempnewMesh;

#ifdef OBJL_CONSOLE_OUTPUT
    const unsigned int outputEveryNth = 1000;
    unsigned int outputIndicator = outputEveryNth;
#endif

    std::string curline;
    while (std::getline(file, curline))
    {
#ifdef OBJL_CONSOLE_OUTPUT
        if ((outputIndicator = ((outputIndicator + 1) % outputEveryNth)) == 1)
        {
            if (!newMeshname.empty())
            {
                std::cout
                    << "\r- " << newMeshname
                    << "\t| vertices > " << Positions.size()
                    << "\t| texcoords > " << TCoords.size()
                    << "\t| normals > " << Normals.size()
                    << "\t| triangles > " << (Vertices.size() / 3)
                    << (!newMeshMatNames.empty() ? "\t| material: " + newMeshMatNames.back() : "");
            }
        }
#endif

        // Generate a newMesh Object or Prepare for an object to be created
        if (firstToken(curline) == "o" || firstToken(curline) == "g" || curline[0] == 'g')
        {
            if (!listening)
            {
                listening = true;

                if (firstToken(curline) == "o" || firstToken(curline) == "g")
                {
                    newMeshname = tail(curline);
                }
                else
                {
                    newMeshname = "unnamed";
                }
            }
            else
            {
                // Generate the newMesh to put into the array

                if (!Indices.empty() && !Vertices.empty())
                {
                    // Create newMesh
                    tempnewMesh = newMesh(Vertices, Indices);
                    tempnewMesh.newMeshName = newMeshname;

                    // Insert newMesh
                    LoadednewMeshes.push_back(tempnewMesh);

                    // Cleanup
                    Vertices.clear();
                    Indices.clear();
                    newMeshname.clear();

                    newMeshname = tail(curline);
                }
                else
                {
                    if (firstToken(curline) == "o" || firstToken(curline) == "g")
                    {
                        newMeshname = tail(curline);
                    }
                    else
                    {
                        newMeshname = "unnamed";
                    }
                }
            }
#ifdef OBJL_CONSOLE_OUTPUT
            std::cout << std::endl;
            outputIndicator = 0;
#endif
        }
        // Generate a Vertex Position
        if (firstToken(curline) == "v")
        {
            std::vector<std::string> spos;
            glm::vec3 vpos;
            split(tail(curline), spos, " ");

            vpos.x = std::stof(spos[0]);
            vpos.y = std::stof(spos[1]);
            vpos.z = std::stof(spos[2]);

            Positions.push_back(vpos);
        }
        // Generate a Vertex Texture Coordinate
        if (firstToken(curline) == "vt")
        {
            std::vector<std::string> stex;
            glm::vec2 vtex;
            split(tail(curline), stex, " ");

            vtex.x = std::stof(stex[0]);
            vtex.y = std::stof(stex[1]);

            TCoords.push_back(vtex);
        }
        // Generate a Vertex Normal;
        if (firstToken(curline) == "vn")
        {
            std::vector<std::string> snor;
            glm::vec3 vnor;
            split(tail(curline), snor, " ");

            vnor.x = std::stof(snor[0]);
            vnor.y = std::stof(snor[1]);
            vnor.z = std::stof(snor[2]);

            Normals.push_back(vnor);
        }
        // Generate a Face (vertices & indices)
        if (firstToken(curline) == "f")
        {
            // Generate the vertices
            std::vector<Vertex> vVerts;
            GenVerticesFromRawOBJ(vVerts, Positions, TCoords, Normals, curline);

            // Add Vertices
            for (int i = 0; i < int(vVerts.size()); i++)
            {
                Vertices.push_back(vVerts[i]);

                LoadedVertices.push_back(vVerts[i]);
            }

            std::vector<unsigned int> iIndices;

            VertexTriangulation(iIndices, vVerts);

            // Add Indices
            for (int i = 0; i < int(iIndices.size()); i++)
            {
                unsigned int indnum = (unsigned int)((Vertices.size()) - vVerts.size()) + iIndices[i];
                Indices.push_back(indnum);

                indnum = (unsigned int)((LoadedVertices.size()) - vVerts.size()) + iIndices[i];
                LoadedIndices.push_back(indnum);
            }
        }
        // Get newMesh Material Name
        if (firstToken(curline) == "usemtl")
        {
            newMeshMatNames.push_back(tail(curline));

            // Create new newMesh, if Material changes within a group
            if (!Indices.empty() && !Vertices.empty())
            {
                // Create newMesh
                tempnewMesh = newMesh(Vertices, Indices);
                tempnewMesh.newMeshName = newMeshname;
                int i = 2;
                while (1) {
                    tempnewMesh.newMeshName = newMeshname + "_" + std::to_string(i);

                    for (auto& m : LoadednewMeshes)
                        if (m.newMeshName == tempnewMesh.newMeshName)
                            continue;
                    break;
                }

                // Insert newMesh
                LoadednewMeshes.push_back(tempnewMesh);

                // Cleanup
                Vertices.clear();
                Indices.clear();
            }

#ifdef OBJL_CONSOLE_OUTPUT
            outputIndicator = 0;
#endif
        }
        // Load Materials
        if (firstToken(curline) == "mtllib")
        {
            // Generate LoadedMaterial

            // Generate a path to the material file
            std::vector<std::string> temp;
            split(Path, temp, "/");

            std::string pathtomat = "";

            if (temp.size() != 1)
            {
                for (int i = 0; i < temp.size() - 1; i++)
                {
                    pathtomat += temp[i] + "/";
                }
            }

            pathtomat += tail(curline);

#ifdef OBJL_CONSOLE_OUTPUT
            std::cout << std::endl << "- find materials in: " << pathtomat << std::endl;
#endif

            // Load Materials
            LoadMaterials(pathtomat);
        }
    }

#ifdef OBJL_CONSOLE_OUTPUT
    std::cout << std::endl;
#endif

    // Deal with last newMesh

    if (!Indices.empty() && !Vertices.empty())
    {
        // Create newMesh
        tempnewMesh = newMesh(Vertices, Indices);
        tempnewMesh.newMeshName = newMeshname;

        // Insert newMesh
        LoadednewMeshes.push_back(tempnewMesh);
    }

    file.close();

    // Set Materials for each newMesh
    for (int i = 0; i < newMeshMatNames.size(); i++)
    {
        std::string matname = newMeshMatNames[i];

        // Find corresponding material name in loaded materials
        // when found copy material variables into newMesh material
        for (int j = 0; j < LoadedMaterials.size(); j++)
        {
            if (LoadedMaterials[j].name == matname)
            {
                LoadednewMeshes[i].newMeshMaterial = LoadedMaterials[j];
                break;
            }
        }
    }

    if (LoadednewMeshes.empty() && LoadedVertices.empty() && LoadedIndices.empty())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Loader::GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
    const std::vector<glm::vec3>& iPositions,
    const std::vector<glm::vec2>& iTCoords,
    const std::vector<glm::vec3>& iNormals,
    std::string icurline)
{
    std::vector<std::string> sface, svert;
    Vertex vVert;
    split(tail(icurline), sface, " ");

    bool noNormal = false;

    // For every given vertex do this
    for (int i = 0; i < int(sface.size()); i++)
    {
        // See What type the vertex is.
        int vtype;

        split(sface[i], svert, "/");

        // Check for just position - v1
        if (svert.size() == 1)
        {
            // Only position
            vtype = 1;
        }

        // Check for position & texture - v1/vt1
        if (svert.size() == 2)
        {
            // Position & Texture
            vtype = 2;
        }

        // Check for Position, Texture and Normal - v1/vt1/vn1
        // or if Position and Normal - v1//vn1
        if (svert.size() == 3)
        {
            if (svert[1] != "")
            {
                // Position, Texture, and Normal
                vtype = 4;
            }
            else
            {
                // Position & Normal
                vtype = 3;
            }
        }

        // Calculate and store the vertex
        switch (vtype)
        {
        case 1: // P
        {
            vVert.Position = getElement(iPositions, svert[0]);
            vVert.TextureCoordinate = glm::vec2(0, 0);
            noNormal = true;
            oVerts.push_back(vVert);
            break;
        }
        case 2: // P/T
        {
            vVert.Position = getElement(iPositions, svert[0]);
            vVert.TextureCoordinate = getElement(iTCoords, svert[1]);
            noNormal = true;
            oVerts.push_back(vVert);
            break;
        }
        case 3: // P//N
        {
            vVert.Position = getElement(iPositions, svert[0]);
            vVert.TextureCoordinate = glm::vec2(0, 0);
            vVert.Normal = getElement(iNormals, svert[2]);
            oVerts.push_back(vVert);
            break;
        }
        case 4: // P/T/N
        {
            vVert.Position = getElement(iPositions, svert[0]);
            vVert.TextureCoordinate = getElement(iTCoords, svert[1]);
            vVert.Normal = getElement(iNormals, svert[2]);
            oVerts.push_back(vVert);
            break;
        }
        default:
        {
            break;
        }
        }
    }

    // take care of missing normals
    // these may not be truly acurate but it is the
    // best they get for not compiling a newMesh with normals
    if (noNormal)
    {
        glm::vec3 A = oVerts[0].Position - oVerts[1].Position;
        glm::vec3 B = oVerts[2].Position - oVerts[1].Position;

        glm::vec3 normal = CrossV3(A, B);

        for (int i = 0; i < int(oVerts.size()); i++)
        {
            oVerts[i].Normal = normal;
        }
    }
}

// Triangulate a list of vertices into a face by printing
//	inducies corresponding with triangles within it
void Loader::VertexTriangulation(std::vector<unsigned int>& oIndices, const std::vector<Vertex>& iVerts)
{
    // If there are 2 or less verts,
    // no triangle can be created,
    // so exit
    if (iVerts.size() < 3)
    {
        return;
    }
    // If it is a triangle no need to calculate it
    if (iVerts.size() == 3)
    {
        oIndices.push_back(0);
        oIndices.push_back(1);
        oIndices.push_back(2);
        return;
    }

    // Create a list of vertices
    std::vector<Vertex> tVerts = iVerts;

    while (true)
    {
        // For every vertex
        for (int i = 0; i < int(tVerts.size()); i++)
        {
            // pPrev = the previous vertex in the list
            Vertex pPrev;
            if (i == 0)
            {
                pPrev = tVerts[tVerts.size() - 1];
            }
            else
            {
                pPrev = tVerts[i - 1];
            }

            // pCur = the current vertex;
            Vertex pCur = tVerts[i];

            // pNext = the next vertex in the list
            Vertex pNext;
            if (i == tVerts.size() - 1)
            {
                pNext = tVerts[0];
            }
            else
            {
                pNext = tVerts[i + 1];
            }

            // Check to see if there are only 3 verts left
            // if so this is the last triangle
            if (tVerts.size() == 3)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(tVerts.size()); j++)
                {
                    if (iVerts[j].Position == pCur.Position)
                        oIndices.push_back(j);
                    if (iVerts[j].Position == pPrev.Position)
                        oIndices.push_back(j);
                    if (iVerts[j].Position == pNext.Position)
                        oIndices.push_back(j);
                }

                tVerts.clear();
                break;
            }
            if (tVerts.size() == 4)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(iVerts.size()); j++)
                {
                    if (iVerts[j].Position == pCur.Position)
                        oIndices.push_back(j);
                    if (iVerts[j].Position == pPrev.Position)
                        oIndices.push_back(j);
                    if (iVerts[j].Position == pNext.Position)
                        oIndices.push_back(j);
                }

                glm::vec3 tempVec;
                for (int j = 0; j < int(tVerts.size()); j++)
                {
                    if (tVerts[j].Position != pCur.Position
                        && tVerts[j].Position != pPrev.Position
                        && tVerts[j].Position != pNext.Position)
                    {
                        tempVec = tVerts[j].Position;
                        break;
                    }
                }

                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(iVerts.size()); j++)
                {
                    if (iVerts[j].Position == pPrev.Position)
                        oIndices.push_back(j);
                    if (iVerts[j].Position == pNext.Position)
                        oIndices.push_back(j);
                    if (iVerts[j].Position == tempVec)
                        oIndices.push_back(j);
                }

                tVerts.clear();
                break;
            }

            // If Vertex is not an interior vertex
            float angle = AngleBetweenV3(pPrev.Position - pCur.Position, pNext.Position - pCur.Position) * (180 / 3.14159265359);
            if (angle <= 0 && angle >= 180)
                continue;

            // If any vertices are within this triangle
            bool inTri = false;
            for (int j = 0; j < int(iVerts.size()); j++)
            {
                if (inTriangle(iVerts[j].Position, pPrev.Position, pCur.Position, pNext.Position)
                    && iVerts[j].Position != pPrev.Position
                    && iVerts[j].Position != pCur.Position
                    && iVerts[j].Position != pNext.Position)
                {
                    inTri = true;
                    break;
                }
            }
            if (inTri)
                continue;

            // Create a triangle from pCur, pPrev, pNext
            for (int j = 0; j < int(iVerts.size()); j++)
            {
                if (iVerts[j].Position == pCur.Position)
                    oIndices.push_back(j);
                if (iVerts[j].Position == pPrev.Position)
                    oIndices.push_back(j);
                if (iVerts[j].Position == pNext.Position)
                    oIndices.push_back(j);
            }

            // Delete pCur from the list
            for (int j = 0; j < int(tVerts.size()); j++)
            {
                if (tVerts[j].Position == pCur.Position)
                {
                    tVerts.erase(tVerts.begin() + j);
                    break;
                }
            }

            // reset i to the start
            // -1 since loop will add 1 to it
            i = -1;
        }

        // if no triangles were created
        if (oIndices.size() == 0)
            break;

        // if no more vertices
        if (tVerts.size() == 0)
            break;
    }
}

// Load Materials from .mtl file
bool Loader::LoadMaterials(std::string path)
{
    // If the file is not a material file return false
    if (path.substr(path.size() - 4, path.size()) != ".mtl")
        return false;

    std::ifstream file(path);

    // If the file is not found return false
    if (!file.is_open())
        return false;

    Material tempMaterial;

    bool listening = false;

    // Go through each line looking for material variables
    std::string curline;
    while (std::getline(file, curline))
    {
        // new material and material name
        if (firstToken(curline) == "newmtl")
        {
            if (!listening)
            {
                listening = true;

                if (curline.size() > 7)
                {
                    tempMaterial.name = tail(curline);
                }
                else
                {
                    tempMaterial.name = "none";
                }
            }
            else
            {
                // Generate the material

                // Push Back loaded Material
                LoadedMaterials.push_back(tempMaterial);

                // Clear Loaded Material
                tempMaterial = Material();

                if (curline.size() > 7)
                {
                    tempMaterial.name = tail(curline);
                }
                else
                {
                    tempMaterial.name = "none";
                }
            }
        }
        // Ambient Color
        if (firstToken(curline) == "Ka")
        {
            std::vector<std::string> temp;
            split(tail(curline), temp, " ");

            if (temp.size() != 3)
                continue;

            tempMaterial.Ka.x = std::stof(temp[0]);
            tempMaterial.Ka.y = std::stof(temp[1]);
            tempMaterial.Ka.z = std::stof(temp[2]);
        }
        // Diffuse Color
        if (firstToken(curline) == "Kd")
        {
            std::vector<std::string> temp;
            split(tail(curline), temp, " ");

            if (temp.size() != 3)
                continue;

            tempMaterial.Kd.x = std::stof(temp[0]);
            tempMaterial.Kd.y = std::stof(temp[1]);
            tempMaterial.Kd.z = std::stof(temp[2]);
        }
        // Specular Color
        if (firstToken(curline) == "Ks")
        {
            std::vector<std::string> temp;
            split(tail(curline), temp, " ");

            if (temp.size() != 3)
                continue;

            tempMaterial.Ks.x = std::stof(temp[0]);
            tempMaterial.Ks.y = std::stof(temp[1]);
            tempMaterial.Ks.z = std::stof(temp[2]);
        }
        // Specular Exponent
        if (firstToken(curline) == "Ns")
        {
            tempMaterial.Ns = std::stof(tail(curline));
        }
        // Optical Density
        if (firstToken(curline) == "Ni")
        {
            tempMaterial.Ni = std::stof(tail(curline));
        }
        // Dissolve
        if (firstToken(curline) == "d")
        {
            tempMaterial.d = std::stof(tail(curline));
        }
        // Illumination
        if (firstToken(curline) == "illum")
        {
            tempMaterial.illum = std::stoi(tail(curline));
        }
        // Ambient Texture Map
        if (firstToken(curline) == "map_Ka")
        {
            tempMaterial.map_Ka = tail(curline);
        }
        // Diffuse Texture Map
        if (firstToken(curline) == "map_Kd")
        {
            tempMaterial.map_Kd = tail(curline);
        }
        // Specular Texture Map
        if (firstToken(curline) == "map_Ks")
        {
            tempMaterial.map_Ks = tail(curline);
        }
        // Specular Hightlight Map
        if (firstToken(curline) == "map_Ns")
        {
            tempMaterial.map_Ns = tail(curline);
        }
        // Alpha Texture Map
        if (firstToken(curline) == "map_d")
        {
            tempMaterial.map_d = tail(curline);
        }
        // Bump Map
        if (firstToken(curline) == "map_Bump" || firstToken(curline) == "map_bump" || firstToken(curline) == "bump")
        {
            tempMaterial.map_bump = tail(curline);
        }
    }

    // Deal with last material

    // Push Back loaded Material
    LoadedMaterials.push_back(tempMaterial);

    // Test to see if anything was loaded
    // If not return false
    if (LoadedMaterials.empty())
        return false;
    // If so return true
    else
        return true;
}