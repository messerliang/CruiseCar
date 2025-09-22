#include "OsmXmlData.h"
#include <string>
#include <unordered_set>

namespace Osm {


    float Scale(float f) {

        f = f * 100;
        return (f - ((int)f)) * 10;
    }


    std::vector<VertexAttributes> MergeAdjacentVertices(const std::vector<VertexAttributes>& Vertices, float Epsilon)
    {
        std::unordered_set<std::string> ExistVertices;
        std::vector<VertexAttributes> Result;
        for (auto& Vertex : Vertices)
        {
            int X = int(std::round(Vertex.Position.x / Epsilon));
            int Y = int(std::round(Vertex.Position.y / Epsilon));
            int Z = int(std::round(Vertex.Position.z / Epsilon));
            std::string Key = std::to_string(X) + std::to_string(Y) + std::to_string(Z);
            //if (ExistVertices.count(Key))
            //{
            //    std::cout << "merge\n";
            //    continue;
            //}
            //std::cout << Key << std::endl;
            ExistVertices.insert(Key);
            Result.emplace_back(Vertex);
        }
        //std::cout << "merged size：" << Result.size() << std::endl;
        return Result;
    }

    bool IsPointInTriangle(glm::vec3& P, glm::vec3& A, glm::vec3& B, glm::vec3& C)
    {
        float eps = 0.000001f;
        // Step 1: 检查点是否在三角形所在平面上
        glm::vec3 norm = cross(B - A, C - A);
        glm::vec3 PA = P - A;
        if (std::fabs(glm::dot(norm, PA)) > eps) {
            return false; // 不在平面上
        }
        // 三个边向量
        glm::vec3 AB = B - A;
        glm::vec3 BC = C - B;
        glm::vec3 CA = A - C;

        // 点到顶点的向量
        glm::vec3 AP = P - A;
        glm::vec3 BP = P - B;
        glm::vec3 CP = P - C;

        // 计算叉积，得到三个法向量
        glm::vec3 c1 = glm::normalize(glm::cross(AB, AP));
        glm::vec3 c2 = glm::normalize(glm::cross(BC, BP));
        glm::vec3 c3 = glm::normalize(glm::cross(CA, CP));

        // 计算三角形平面法线
        glm::vec3 n = glm::normalize(cross(AB, C - A));

        // 检查三个叉积是否和法线同向
        double d1 = glm::dot(n, c1);
        double d2 = glm::dot(n, c2);
        double d3 = glm::dot(n, c3);

        return (d1 >= -eps && d2 >= -eps && d3 >= -eps) ||
            (d1 <= eps && d2 <= eps && d3 <= eps);
    }


    std::vector<long long> FindEar(const std::vector<VertexAttributes>& Vertices, const std::unordered_set<long long> ExistIndices, long long Index, glm::vec3 DirectionalNorm)
    {
        

        std::vector<long long> Result;
        long long Number = Vertices.size();
        if (ExistIndices.count(Index) == 0)
        {
            return Result;
        }
        // 找相邻的上一个顶点，如果上一个顶点已经切掉，选上上个
        long long IndexPre = (Index - 1 + Number) % Number;
        while (IndexPre >= 0 && ExistIndices.count(IndexPre) == 0)
        {
            IndexPre--;
        }
        if (IndexPre < 0 || IndexPre == Index) { return {}; }

        // 找相邻的下一个顶点，如果下一个顶点已经切掉，选下下个
        long long IndexNxt = (Index + 1) % Number;
        while (IndexNxt < Number && ExistIndices.count(IndexNxt) == 0)
        {
            IndexNxt++;
        }
        if (IndexNxt >= Number || IndexNxt == Index) { return {}; }



        // 开始进行判断
        glm::vec3 P0 = Vertices[IndexPre].Position;
        glm::vec3 P1 = Vertices[Index].Position;
        glm::vec3 P2 = Vertices[IndexNxt].Position;

        glm::vec3 CrossDir = glm::normalize(glm::cross(P2 - P1, P0 - P1));
        if (glm::dot(CrossDir, DirectionalNorm) < 0) { return {}; } // 不凸三角形

        for (auto OtherIndex : ExistIndices)
        {
            if (OtherIndex == Index || OtherIndex == IndexPre || OtherIndex == IndexNxt) { continue; }

            glm::vec3 Point = Vertices[OtherIndex].Position;
            if (IsPointInTriangle(Point, P0, P1, P2)) // 有其他点在当前三角形里面
            {
                return {};
            }
        }
        Result.push_back(IndexPre);
        Result.push_back(Index);
        Result.push_back(IndexNxt);
        return Result;
    }

    // 输入顶点信息，起始位置，以及围成多边形的顶点数量，输出构成三角形的 index 数组
    std::vector<unsigned int> EarClip(std::vector<VertexAttributes>& Vertices, unsigned int IndexOffset)
    {

        std::vector<VertexAttributes> MergedVertices = MergeAdjacentVertices(Vertices, 0.01f);

        std::vector<unsigned int> Result;
        int Number = MergedVertices.size();
        std::unordered_set<long long> Exist;
        // 根据顺时针还是逆时针，计算法线方向
        glm::vec3 DirectionalNorm(0);
        
        for (int i = 0; i < Number; ++i)
        {
            glm::vec3 P1= MergedVertices[i].Position;
            glm::vec3 P2= MergedVertices[(i + 1) % Number].Position;
            DirectionalNorm += 0.5f * glm::cross(P1, P2);
        }
        DirectionalNorm = glm::normalize(DirectionalNorm);
 

        for (int i = 0; i < MergedVertices.size(); ++i)
        {
            Exist.insert(i);
        }

        int MaxInteration = 300;

        while (Exist.size() >= 3)
        {
            MaxInteration--;
            for (int StartPos = 0; StartPos < Number; ++StartPos)
            {
                if (Exist.count(StartPos) == 0) { continue; }
                std::vector<long long> Triangle = FindEar(MergedVertices, Exist, StartPos, DirectionalNorm);
                if (0 == Triangle.size()) { continue; }

                Exist.erase(StartPos);
                Result.push_back(Triangle[0] + IndexOffset);
                Result.push_back(Triangle[1] + IndexOffset);
                Result.push_back(Triangle[2] + IndexOffset);
            }

            if (!MaxInteration)
            {
                break;
            }

        }

        if (!MaxInteration)
        {
            for (auto& tmp : MergedVertices)
            {
                std::cout << tmp.Position.x << " " << tmp.Position.y << " " << tmp.Position.z << std::endl;
            }
            std::cout << "fail \n";
        }
        return Result;
    }

    std::vector<VertexAttributes> GenerateSideFaces(const std::vector<VertexAttributes>& BottomVertices, const std::vector<VertexAttributes>& TopVertices)
    {
        if (BottomVertices.size() != TopVertices.size() || BottomVertices.size() < 3)
        {
            std::cout << "not match\n";
            return {};
        }
        int Number = BottomVertices.size();
        // 根据顺时针和逆时针，计算法线方向
        glm::vec3 DirectionalNorm(0);

        for (int i = 0; i < Number; ++i)
        {
            glm::vec3 P1 = BottomVertices[i].Position;
            glm::vec3 P2 = BottomVertices[(i + 1) % Number].Position;
            DirectionalNorm += 0.5f * glm::cross(P1, P2);
        }
        DirectionalNorm = glm::normalize(DirectionalNorm);


        std::vector<VertexAttributes> Result;


        for (int Index = 0; Index < Number; ++Index)
        {
            int NxtIndex = (Index + 1) % Number;
            glm::vec3 EdgeDir = BottomVertices[NxtIndex].Position - BottomVertices[Index].Position;
            glm::vec3 Norm = glm::normalize(glm::cross(EdgeDir, DirectionalNorm));
            
            // 第一个三角形
            Result.emplace_back(BottomVertices[Index].Position, Norm);
            Result.emplace_back(TopVertices[Index].Position, Norm);
            Result.emplace_back(BottomVertices[NxtIndex].Position, Norm);
            // 第二个三角形
            Result.emplace_back(TopVertices[Index].Position, Norm);
            Result.emplace_back(BottomVertices[NxtIndex].Position, Norm);
            Result.emplace_back(TopVertices[NxtIndex].Position, Norm);

        }
        return Result;
    }

	int OsmXmlData::ReadXmlFile(const char* Path)
	{

        Nodes.clear();
        NodeIdMap.clear();
        Ways.clear();

		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(Path) != tinyxml2::XML_SUCCESS) {
			std::cerr << "无法打开 XML 文件" << std::endl;
			return -1;
		}

		// 获取根节点
		tinyxml2::XMLElement* root = doc.RootElement();
		tinyxml2::XMLElement* osm = root->FirstChildElement("osm");


        // 遍历所有 user 节点
        for (tinyxml2::XMLElement* node = root->FirstChildElement("node");
            node != nullptr;
            node = node->NextSiblingElement("node"))
        {

            const char* id = node->Attribute("id");

            const char* lat = node->Attribute("lat");
            const char* lon = node->Attribute("lon");

            long long Id = std::atoll(id);
            float latitude = std::atof(lat);
            float longitude = std::atof(lon);
            if (NodeIdMap.count(Id))
            {
                std::cout << Id << " exists " << id << std::endl;
            }
            NodeIdMap[Id] = Nodes.size();
            Nodes.emplace_back(Id, latitude, longitude);
        }

        // 读取 way 信息
        std::string key = "way";
        bool show = true;
        int wayShowCnt = 0;
        for (tinyxml2::XMLElement* way = root->FirstChildElement(key.c_str());
            way != nullptr;
            way = way->NextSiblingElement(key.c_str())
            )
        {
            const char* id = way->Attribute("id");
            std::vector<long long> nodeIds;

            // 读取节点信息
            for (tinyxml2::XMLElement* nd = way->FirstChildElement("nd");
                nd != nullptr;
                nd = nd->NextSiblingElement("nd"))
            {
                const char* ndId = nd->Attribute("ref");
                long long ndIdL = std::atoll(ndId);
                nodeIds.push_back(ndIdL);

            }

            std::unordered_map < std::string, std::string> tags;

            for (tinyxml2::XMLElement* tag = way->FirstChildElement("tag");
                tag != nullptr;
                tag = tag->NextSiblingElement("tag"))
            {
                std::string k = tag->Attribute("k");
                std::string v = tag->Attribute("v");
                tags[k] = v;
            }

            Ways.emplace_back(std::atoll(id), nodeIds, tags);

        }
        return 0;
	}
}