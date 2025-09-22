#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "tinyxml2.h"

namespace Osm{

    struct VertexAttributes {
        glm::vec3 Position;
        glm::vec3 Normal;

        VertexAttributes(const glm::vec3& Pos = glm::vec3(0), const glm::vec3 Norm = glm::vec3(1, 0, 0)) :
            Position(Pos), Normal(Norm) {
        }
    };

    struct Node
    {
        long long Id;
        float Latitude;
        float Longitude;
        Node(long long i = 0, float lat = 0.0f, float lon = 0.0f) :Id(i), Latitude(lat), Longitude(lon) {}
    };

    struct Tag
    {
        std::string Key;
        std::string Value;
    };

    struct Way {
        long long Id;
        std::vector<long long> NodeIds;
        std::unordered_map<std::string, std::string> Tags;
        Way(long long id = 0,
            const std::vector<long long>& nodes = {},
            const std::unordered_map<std::string, std::string>& tags = {}) :Id(id), NodeIds(nodes), Tags(tags) {
        }
    };

    float Scale(float f);

    bool IsPointInTriangle(glm::vec3& P, glm::vec3& A, glm::vec3& B, glm::vec3& C);
    /// <summary>
    ///  ����һ�����±꣬�ҵ����ڵĵ㹹�������Σ������ǰ�ĵ��� Ear ���� ͹�����Σ���û���������������������
    /// </summary>
    /// <param name="Vertices"></param>
    /// <param name="ExistIndices"></param>
    /// <param name="Index"></param>
    /// <returns></returns>
    std::vector<long long> FindEar(const std::vector<VertexAttributes>& Vertices, const std::unordered_set<long long> ExistIndices, long long Index, glm::vec3 DirectionalNorm);

    // ���붥����Ϣ����ʼλ�ã��Լ�Χ�ɶ���εĶ���������������������ε� index ����
    std::vector<unsigned int> EarClip(std::vector<VertexAttributes>& Vertices, unsigned int IndexOffset = 0);

    // ���ײ���Ͷ��������飬���ɲ������������
    std::vector<VertexAttributes> GenerateSideFaces(const std::vector<VertexAttributes>& BottomVertices, const std::vector<VertexAttributes>& TopVertices);

    // ȥ��������Χ�ڵ����ڶ���
    std::vector<VertexAttributes> MergeAdjacentVertices(const std::vector<VertexAttributes>& BottomVertices, float Epsilon=0.001);

    class OsmXmlData
    {
    public:
        std::string XmlFile;

        std::vector<Node> Nodes;
        std::unordered_map<long long, long long> NodeIdMap; // key��id�� value����nodes�е�λ��

        std::vector<Way> Ways;

        int ReadXmlFile(const char* Path);

    };
}



