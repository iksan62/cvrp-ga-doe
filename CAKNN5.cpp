
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <random>
#include <set>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include <tuple>
#include <numeric>

#include <string>

//This is CAKNN5
//------initialization
//------ This part is filled according to benchmark instances

std::vector<std::pair<int, int>> coordinates = { {0, 0}, {615, 190}, {890, 200}, {983, 684}, {959, 743}, {569, 146}, {969, 841}, {607, 256}, {936, 274}, {650, 221}, {943, 547}, {789, 172}, {632, 209}, {849, 163}, {829, 283}, {618, 147}, {925, 627}, {925, 192}, {903, 460}, {666, 368}, {906, 622}, {833, 185}, {411, 136}, {989, 691}, {601, 221}, {429, 500}, {522, 218}, {760, 346}, {674, 183}, {720, 99}, {951, 705}, {898, 154}, {953, 661}, {425, 212}, {775, 159}, {900, 649}, {943, 290}, {724, 315}, {927, 705}, {865, 817}, {872, 161}, {966, 478}, {974, 694}, {669, 159}, {603, 140}, {598, 122}, {715, 230}, {777, 693}, {890, 231}, {775, 230}, {908, 849}, {625, 138}, {490, 185}, {643, 163}, {849, 183}, {998, 675}, {619, 495}, {844, 67}, {861, 276}, {888, 679}, {641, 141}, {869, 766}, {495, 244}, {852, 110}, {976, 143}, {921, 783}, {688, 211}, {857, 199}, {758, 636}, {990, 213}, {973, 806}, {748, 240}, {737, 95}, {933, 664}, {770, 172}, {616, 250}, {641, 146}, {867, 199}, {544, 327}, {711, 277}, {893, 778}, {992, 868}, {685, 11}, {869, 204}, {646, 260}, {849, 185}, {918, 400}, {645, 144}, {923, 339}, {931, 686}, {582, 212}, {1000, 309}, {572, 192}, {676, 183}, {989, 59}, {798, 642}, {582, 39}, {686, 188}, {995, 720}, {802, 185}, {603, 126}, {657, 185}, {574, 195}, {661, 174}, {614, 195}, {961, 162} }; //insert nodes location
std::vector<std::pair<int, int>> demand = { {0, 0}, {1, 81}, {2, 64}, {3, 51}, {4, 61}, {5, 71}, {6, 83}, {7, 92}, {8, 66}, {9, 92}, {10, 78}, {11, 98}, {12, 88}, {13, 70}, {14, 72}, {15, 92}, {16, 84}, {17, 51}, {18, 81}, {19, 71}, {20, 98}, {21, 70}, {22, 53}, {23, 76}, {24, 83}, {25, 83}, {26, 77}, {27, 82}, {28, 88}, {29, 58}, {30, 97}, {31, 53}, {32, 52}, {33, 89}, {34, 87}, {35, 50}, {36, 73}, {37, 58}, {38, 100}, {39, 59}, {40, 67}, {41, 75}, {42, 65}, {43, 80}, {44, 65}, {45, 63}, {46, 59}, {47, 75}, {48, 56}, {49, 76}, {50, 83}, {51, 85}, {52, 96}, {53, 58}, {54, 50}, {55, 54}, {56, 81}, {57, 82}, {58, 99}, {59, 67}, {60, 88}, {61, 92}, {62, 97}, {63, 50}, {64, 62}, {65, 59}, {66, 83}, {67, 86}, {68, 86}, {69, 55}, {70, 99}, {71, 88}, {72, 99}, {73, 80}, {74, 98}, {75, 87}, {76, 81}, {77, 58}, {78, 57}, {79, 87}, {80, 50}, {81, 96}, {82, 63}, {83, 75}, {84, 79}, {85, 100}, {86, 99}, {87, 95}, {88, 57}, {89, 59}, {90, 94}, {91, 91}, {92, 85}, {93, 95}, {94, 50}, {95, 53}, {96, 66}, {97, 56}, {98, 63}, {99, 86}, {100, 71}, {101, 50}, {102, 65}, {103, 78}, {104, 56}, {105, 72} };// insert customers demand
int vehicleCapacity = 600; // insert capacity of vehicles
int numVehicle = 14; // insert humber of vehicle
std::string benchmarkID = "X-n106-k14"; //insert ID of benchmark

//------ This part is filled according to GA Parameter
int populationSize = 30; // insert population size
int generations = 10; // number of generations
double crossoverRate = 0.6; // insert crossover rate
double mutationRate = 0.01; // insert mutation rate
double breederRate = 0.05; // insert breeder rate
int run = 5; //insert number of run

int numCities = coordinates.size();
int chromosomeSize = numCities + numVehicle;
int hammingDistance = 0; int totalhammingDistance = 0; double geneticDiversity;
int multiply = 100;
int GEN2 = 0; int genB = 0;
int K = 2;

struct Individual {
    std::vector<int> route;
    double fitness = 0.0;
    double totalTravelDistance = 0.0;
};


struct IndividuDetail {
    int feasibility;
    int startNode;
    int endNode;
    int currentLoad;
    double travelDistance;

    IndividuDetail(int feas = 0, int sNode = 0, int eNode = 0, int cLoad = 0, double tDist = 0.0)
        : feasibility(feas), startNode(sNode), endNode(eNode), currentLoad(cLoad), travelDistance(tDist) {
    }
};

std::vector<Individual> population(populationSize);
std::vector<Individual> bestOverall(populationSize);
std::vector<std::vector<IndividuDetail>> individuDetail(populationSize, std::vector<IndividuDetail>(numVehicle));
std::vector<Individual> newPopulation(populationSize);
std::vector<std::vector<IndividuDetail>> newPopulationDetail(populationSize, std::vector<IndividuDetail>(numVehicle));
std::vector<int> processSuccessLabel(populationSize, 5);
std::vector<std::vector<double>> distanceMatrix(numCities, std::vector<double>(numCities));
std::vector<int> servedCities(numCities, 0);
int servedCount;
std::vector<int> checker(chromosomeSize, 0); int capacityChecker;
std::vector<std::tuple< int, int, int, int, double>> routeDetailChecker(numVehicle, { 0, 0, 0, 0, 0.0 });
std::vector<std::vector<double>> calculateDistanceMatrix(const std::vector<std::pair<int, int>>& coordinates, const std::string& benchmarkID);
double calculateDistance(const std::pair<int, int>& point1, const std::pair<int, int>& point2);
int generateRandomNumber(int numCities, const std::set<int>& forbiddenNumbers);
void printOnlyServedCities(const std::vector<int>& servedCities);
void printOnlyNotServedCities(const std::vector<int>& servedCities, const std::vector<std::pair<int, int>>& demanddescendingsort);
bool allCitiesServed(const std::vector<int>& servedCities, int numCities);
bool hasUnservedCities(const std::vector<int>& servedCities);
int countServedCities(const std::vector<int>& servedCities);
using namespace std;
void sortCitiesByDemand(vector<pair<int, int>>& cities);
void writePopulationToCSV(int gen, std::ofstream& file, const std::vector<Individual>& population);
void writeFitnessToCSV(int gen, std::ofstream& file, const std::vector<Individual>& population);
void writeIndividuDetailToCSV(std::ofstream& file, const std::vector<std::vector<IndividuDetail>>& individuDetail);
double calculateDistance(const vector<int>& route, const vector<vector<double>>& distanceMatrix);
int calculateHammingDistance(const std::vector<int>& a, const std::vector<int>& b);
double calculateGeneticDiversity(const std::vector<Individual>& population);
int countVisitedCities(const Individual& individual);

void geneticAlgorithm(int populationSize, int chromosomeSize, int numCities, int numVehicle, int generations, int gen, const std::vector<std::pair<int, int>>& coordinates, const std::vector<std::pair<int, int>>& demand, int vehicleCapacity, const std::string& benchmarkID, const std::vector<std::vector<double>>& distanceMatrix, std::ofstream& MyExcelFile2);
void fillRouteDetail(const std::vector<std::vector<double>>& distanceMatrix);
void fillIndividuDetail();
std::vector<int> getNodesSortedByProximity(int nodeA, const std::vector<std::vector<double>>& distanceMatrix);
void allNodesServedChecker();
void capacityConstraintChecker();
void findBestIndividual(const std::vector<Individual>& population);

//-----------------------------------------

std::vector<int> determineEligibleNodes(
    int available, int previousNode, int numCities,
    const std::vector<std::vector<double>>& distanceMatrix,
    const std::vector<std::pair<int, int>>& demand,
    const std::vector<int>& servedCities)
{
    std::vector<int> eligibleNodes;
    std::vector<std::pair<int, double>> nodeDistancePairs;
    for (const auto& d : demand) {
        int node = d.first;
        int nodeDemand = d.second;
        if (node != previousNode && nodeDemand <= available && servedCities[node] == 0) {
            double distance = distanceMatrix[previousNode][node];
            nodeDistancePairs.emplace_back(node, distance);
        }
    }
    std::sort(nodeDistancePairs.begin(), nodeDistancePairs.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
        });
    for (const auto& pair : nodeDistancePairs) {
        eligibleNodes.push_back(pair.first);
    }
    return eligibleNodes;
}

std::vector<Individual> initializePopulation(
    int populationSize, int numCities, int numVehicle, int vehicleCapacity,
    const std::vector<std::pair<int, int>>& demand,
    const std::vector<std::pair<int, int>>& coordinates,
    const std::string& benchmarkID,
    const std::vector<std::vector<double>>& distanceMatrix)
{
    int chromosomeSize = numCities + numVehicle;
    std::vector<Individual> population(populationSize);
    std::vector<std::vector<IndividuDetail>> individuDetail(populationSize, std::vector<IndividuDetail>(numVehicle));
    std::vector<int> servedCities(numCities, 0);

    for (int i = 0; i < populationSize; ++i) {
        population[i].route = std::vector<int>(chromosomeSize, 0);
        population[i].fitness = 0.0;
        population[i].totalTravelDistance = 0.0;
        std::fill(servedCities.begin(), servedCities.end(), 0);
        std::fill(individuDetail[i].begin(), individuDetail[i].end(), IndividuDetail(0, 0, 0, 0, 0.0));
        std::vector<std::tuple<int, int, int, int, int, double>> routeDetail(numVehicle, { 0, 0, 0, 0, 0, 0 });
        std::vector<int> temporaryRouteChecker(chromosomeSize, 0);
        std::vector<std::tuple<int, int, int, int, int, double>> routeDetailTemporary(numVehicle, { 0, 0, 0, 0, 0, 0 });
        vector<pair<int, int>> demanddescendingsort = demand;
        int iterationInitialPopulation = 0;
        int maxIterationInitialPopulation = 10;
        int repairingIteration = 0;
        int maxRepairingIteration = 3 * numVehicle;
        int servedCount = 0;
        do {
            repairingIteration = 0;
            std::fill(servedCities.begin(), servedCities.end(), 0);
            do {
                std::fill(servedCities.begin(), servedCities.end(), 0);
                std::fill(routeDetail.begin(), routeDetail.end(), std::make_tuple(0, 0, 0, 0, 0, 0));
                std::fill(population[i].route.begin(), population[i].route.end(), 0);
                int j = 0, v = 0;
                int currentLoad = 0;
                double travelDistance = 0.0;
                for (v = 0; v < numVehicle; ++v) {
                    int routeNum = v;
                    int startNode = j;
                    population[i].route[j] = 0;
                    servedCities[0] = 1;
                    int previousNode = 0;
                    ++j;
                    currentLoad = 0;
                    travelDistance = 0.0;
                    std::vector<int> eligibleNodes = determineEligibleNodes(vehicleCapacity - currentLoad, previousNode, numCities, distanceMatrix, demand, servedCities);
                    while (!eligibleNodes.empty()) {
                        int kSize = std::min(K, static_cast<int>(eligibleNodes.size()));
                        static std::random_device rd;
                        static std::mt19937 gen(rd());
                        std::uniform_int_distribution<> distrib(0, kSize - 1);
                        int selectedIndex = distrib(gen);
                        int nextNode = eligibleNodes[selectedIndex];
                        population[i].route[j] = nextNode;
                        servedCities[nextNode] = 1;
                        travelDistance += distanceMatrix[previousNode][nextNode];
                        currentLoad += demand[nextNode].second;
                        previousNode = nextNode;
                        ++j;
                        eligibleNodes = determineEligibleNodes(vehicleCapacity - currentLoad, previousNode, numCities, distanceMatrix, demand, servedCities);
                        servedCount = countServedCities(servedCities);
                    }
                    population[i].route[j] = 0;
                    travelDistance += distanceMatrix[previousNode][0];
                    int endNode = j;
                    int feasibility = (currentLoad == vehicleCapacity) ? 1 : 0;
                    routeDetail[v] = std::make_tuple(routeNum, feasibility, startNode, endNode, currentLoad, travelDistance);
                }
                servedCount = countServedCities(servedCities);
                ++iterationInitialPopulation;
            } while (servedCount != numCities);
        } while (servedCount != numCities);
        for (int v = 0; v < numVehicle; ++v) {
            individuDetail[i][v] = IndividuDetail(
                std::get<1>(routeDetail[v]),
                std::get<2>(routeDetail[v]),
                std::get<3>(routeDetail[v]),
                std::get<4>(routeDetail[v]),
                std::get<5>(routeDetail[v])
            );
        }
    }
    return population;
}

void calculateFitness(std::vector<Individual>& population, const std::vector<std::vector<double>>& distanceMatrix) {
    for (Individual& ind : population) {
        ind.fitness = 0.0;
        ind.totalTravelDistance = 0.0;
        for (size_t i = 0; i < ind.route.size() - 1; ++i) {
            int from = ind.route[i];
            int to = ind.route[i + 1];
            double distance = distanceMatrix[from][to];
            ind.totalTravelDistance += distance;
        }
        ind.fitness = 1.0 / (1.0 + ind.totalTravelDistance);
    }
}

Individual rouletteWheelSelection(const std::vector<Individual>& population) {
    double totalFitness = 0.0;
    for (const Individual& ind : population) {
        totalFitness += ind.fitness;
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, totalFitness);
    double randomValue = distrib(gen);
    double cumulativeSum = 0.0;
    for (const Individual& ind : population) {
        cumulativeSum += ind.fitness;
        if (cumulativeSum >= randomValue) {
            return ind;
        }
    }
    return population.back();
}

void proximityConstrainedSegmentCrossover() {
    for (int i = 0; i < newPopulation.size(); ++i) {
        newPopulation[i] = Individual();
        std::fill(newPopulation[i].route.begin(), newPopulation[i].route.end(), 0);
    }
    double maxFitness = 0.0;
    for (const Individual& ind : population) {
        if (ind.fitness > maxFitness) {
            maxFitness = ind.fitness;
        }
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, 1.0);
    std::vector<double> randomCross(populationSize);
    for (int i = 0; i < populationSize; ++i) {
        randomCross[i] = distrib(gen);
    }

    std::vector<int> parent1(chromosomeSize, 0);
    std::vector<int> parent2(chromosomeSize, 0);
    std::vector<std::tuple< int, int, int, int, double>> routeDetailParent1(numVehicle, { 0, 0, 0, 0, 0 });
    int feasibilityParent1, startNodeParent1, endNodeParent1, currentLoadParent1; double travelDistanceParent1;
    std::vector<std::tuple< int, int, int, int, double>> routeDetailParent2(numVehicle, { 0, 0, 0, 0, 0 });
    int feasibilityParent2, startNodeParent2, endNodeParent2, currentLoadParent2; double travelDistanceParent2;
    std::vector<int> parentTemporary1(chromosomeSize, 0);
    std::vector<int> parentTemporary2(chromosomeSize, 0);
    std::vector<std::tuple< int, int, int, int, double>> routeDetailParentTemporary1(numVehicle, { 0, 0, 0, 0, 0 });
    int feasibilityParentTemporary1, startNodeParentTemporary1, endNodeParentTemporary1, currentLoadParentTemporary1; double travelDistanceParentTemporary1;
    std::vector<std::tuple< int, int, int, int, double>> routeDetailParentTemporary2(numVehicle, { 0, 0, 0, 0, 0 });
    int feasibilityParentTemporary2, startNodeParentTemporary2, endNodeParentTemporary2, currentLoadParentTemporary2; double travelDistanceParentTemporary2;

    int selectparent1 = -1, selectparent2 = -1, i = 0;
    while (i < populationSize)
    {
        while (selectparent1 == -1 && i < populationSize) {
            if (randomCross[i] <= crossoverRate) {
                selectparent1 = i;
                parent1 = population[i].route;
                for (int v = 0; v < numVehicle; ++v) {
                    routeDetailParent1[v] = std::make_tuple(
                        individuDetail[i][v].feasibility,
                        individuDetail[i][v].startNode,
                        individuDetail[i][v].endNode,
                        individuDetail[i][v].currentLoad,
                        individuDetail[i][v].travelDistance
                    );
                }

            }

            else {
                newPopulation[i] = population[i];
                newPopulationDetail[i] = individuDetail[i];
                processSuccessLabel[i] = 0;
            }
            i = i + 1;
        }
        while (selectparent2 == -1 && i < populationSize) {
            if (randomCross[i] <= crossoverRate) {
                selectparent2 = i;
                parent2 = population[i].route;
                for (int v = 0; v < numVehicle; ++v) {
                    routeDetailParent2[v] = std::make_tuple(
                        individuDetail[i][v].feasibility,
                        individuDetail[i][v].startNode,
                        individuDetail[i][v].endNode,
                        individuDetail[i][v].currentLoad,
                        individuDetail[i][v].travelDistance
                    );
                }

            }
            else {
                newPopulation[i] = population[i];
                newPopulationDetail[i] = individuDetail[i];
                processSuccessLabel[i] = 0;
            }
            i = i + 1;
        }
        if (selectparent1 != -1 && selectparent2 != -1)
        {
            std::uniform_int_distribution<> distrib(0, numVehicle - 1);
            int selectedRouteCrossover = distrib(gen);
            std::fill(servedCities.begin(), servedCities.end(), 0);
            std::fill(parentTemporary1.begin(), parentTemporary1.end(), 0);
            std::fill(parentTemporary2.begin(), parentTemporary2.end(), 0);
            int startNode, endNode;
            startNode = std::get<1>(routeDetailParent2[selectedRouteCrossover]);
            endNode = std::get<2>(routeDetailParent2[selectedRouteCrossover]);

            for (int j = startNode; j <= endNode; ++j) {
                servedCities[parent2[j]] = 1;
            }
            int nodeParent = -1, jparent = 0, node = -1, jOffspring;


            int j = 0;
            for (int v = 0; v < numVehicle; ++v) {

                if (v == selectedRouteCrossover) {
                    startNode = std::get<1>(routeDetailParent2[selectedRouteCrossover]);
                    endNode = std::get<2>(routeDetailParent2[selectedRouteCrossover]);
                    jparent = startNode;
                    for (jparent = startNode; jparent < endNode; ++jparent) {
                        parentTemporary1[j] = parent2[jparent];
                        ++j;
                    }
                }

                else {
                    startNode = std::get<1>(routeDetailParent1[v]);
                    endNode = std::get<2>(routeDetailParent1[v]);
                    jparent = startNode;

                    for (jparent = startNode; jparent < endNode; ++jparent) {
                        if (parent1[jparent] == 0) {
                            parentTemporary1[j] = parent1[jparent];
                            ++j;
                        }

                        if (servedCities[parent1[jparent]] == 0) {
                            parentTemporary1[j] = parent1[jparent];
                            servedCities[parent1[jparent]] = 1;
                            ++j;
                        }
                    }
                }
            }
            checker = parentTemporary1;
            fillRouteDetail(distanceMatrix);
            routeDetailParentTemporary1 = routeDetailChecker;
            std::vector<std::pair<int, int>> notServedNode(numCities, { -1, -1 }); int selectedNearestNode;
            int nCities = 0;
            for (int cities = 0; cities < numCities; ++cities) {
                if (servedCities[cities] == 0) {
                    notServedNode[nCities].first = cities;
                    ++nCities;
                }
            }
            nCities = 0;
            while (notServedNode[nCities].first != -1) {
                int nodeA = notServedNode[nCities].first;
                std::vector<int> sortedNodes = getNodesSortedByProximity(nodeA, distanceMatrix);
                int nearestNode;
                int cities = 0, j;
                while (cities < numCities && notServedNode[nCities].second == -1)
                {
                    int v = -1, numRouteNearestNode = -1;
                    if (cities >= numCities) {
                        std::cerr << "Error: cities index (" << cities << ") out of range! Max index: " << numCities - 1 << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    if (sortedNodes[cities] == 0 && cities < numCities - 1) {
                        ++cities;
                    }
                    else if (sortedNodes[cities] != 0) {
                        nearestNode = sortedNodes[cities];
                        if (servedCities[nearestNode] == 1) {
                            j = 0;
                            while (parentTemporary1[j] != nearestNode) {
                                if (parentTemporary1[j] == 0) { v = v + 1; }
                                ++j;
                                if (j >= parentTemporary1.size()) {
                                    std::cerr << "Error: j index (" << j << ") out of range! Max index: " << parentTemporary1.size() - 1 << std::endl;
                                    exit(EXIT_FAILURE);
                                }
                            }
                            numRouteNearestNode = v;
                            if (numRouteNearestNode < 0 || numRouteNearestNode >= routeDetailParentTemporary1.size()) {
                                std::cerr << "Error: numRouteNearestNode (" << numRouteNearestNode << ") out of range! Max index: " << routeDetailParentTemporary1.size() - 1 << std::endl;
                                exit(EXIT_FAILURE);
                            }
                            int availableCapacity = vehicleCapacity - std::get<3>(routeDetailParentTemporary1[numRouteNearestNode]);
                            if (nodeA >= demand.size()) {
                                std::cerr << "Error: nodeA (" << nodeA << ") out of range! Max index: " << demand.size() - 1 << std::endl;
                                exit(EXIT_FAILURE);
                            }
                            if (cities == numCities - 1) { break; }
                            if (demand[nodeA].second <= availableCapacity) {
                                if (nCities >= notServedNode.size()) {
                                    std::cerr << "Error: nCities (" << nCities << ") out of range! Max index: " << notServedNode.size() - 1 << std::endl;
                                    exit(EXIT_FAILURE);
                                }

                                notServedNode[nCities].second = nearestNode;
                                servedCities[nearestNode] = 1;
                                std::get<3>(routeDetailParentTemporary1[numRouteNearestNode]) = std::get<3>(routeDetailParentTemporary1[numRouteNearestNode]) + demand[nodeA].second;
                                jparent = 0;
                                for (int j = 0; j < chromosomeSize; ++j)
                                {
                                    if (parentTemporary1[jparent] == nearestNode)
                                    {
                                        parentTemporary2[j] = nearestNode;
                                        ++j;
                                        parentTemporary2[j] = nodeA;
                                        ++j; ++jparent;
                                    }
                                    parentTemporary2[j] = parentTemporary1[jparent];
                                    ++jparent;
                                }
                                parentTemporary1 = parentTemporary2;
                            }
                            else {
                                if (cities < numCities - 1) {
                                    ++cities;
                                }
                                else { break; }

                            }
                        }
                        else {
                            if (cities < numCities - 1) {
                                ++cities;
                            }
                            else { break; }
                        }
                    }
                    else {
                        break;
                    }
                    if (cities == numCities - 1) { break; }
                }
                ++nCities;
            }
            checker = parentTemporary1;
            fillRouteDetail(distanceMatrix);
            routeDetailParentTemporary1 = routeDetailChecker;
            parentTemporary1 = checker;
            allNodesServedChecker();
            routeDetailChecker = routeDetailParentTemporary1;
            capacityConstraintChecker();
            if (servedCount == numCities && capacityChecker == 1) {
                processSuccessLabel[selectparent1] = 1;
                newPopulation[selectparent1].route = parentTemporary1;
                std::copy(parentTemporary1.begin(), parentTemporary1.end(), newPopulation[selectparent1].route.begin());
                for (int v = 0; v < numVehicle; ++v) {
                    int feas = std::get<0>(routeDetailParentTemporary1[v]);
                    int sNode = std::get<1>(routeDetailParentTemporary1[v]);
                    int eNode = std::get<2>(routeDetailParentTemporary1[v]);
                    int cLoad = std::get<3>(routeDetailParentTemporary1[v]);
                    double tDist = std::get<4>(routeDetailParentTemporary1[v]);
                    newPopulationDetail[selectparent1][v] = IndividuDetail(feas, sNode, eNode, cLoad, tDist);
                }
            }
            else {
                processSuccessLabel[selectparent1] = -1;
                newPopulation[selectparent1] = population[selectparent1];
                newPopulationDetail[selectparent1] = individuDetail[selectparent1];
            }
            std::fill(servedCities.begin(), servedCities.end(), 0);
            std::fill(parentTemporary1.begin(), parentTemporary1.end(), 0);
            std::fill(parentTemporary2.begin(), parentTemporary2.end(), 0);
            startNode, endNode;
            startNode = std::get<1>(routeDetailParent1[selectedRouteCrossover]);
            endNode = std::get<2>(routeDetailParent1[selectedRouteCrossover]);
            for (int j = startNode; j <= endNode; ++j) {
                servedCities[parent1[j]] = 1;
            }
            nodeParent = -1, jparent = 0, node = -1, jOffspring;


            j = 0;
            for (int v = 0; v < numVehicle; ++v) {
                if (v == selectedRouteCrossover) {
                    startNode = std::get<1>(routeDetailParent1[selectedRouteCrossover]);
                    endNode = std::get<2>(routeDetailParent1[selectedRouteCrossover]);
                    jparent = startNode;
                    for (jparent = startNode; jparent < endNode; ++jparent) {
                        parentTemporary1[j] = parent1[jparent];
                        ++j;
                    }
                }

                else {
                    startNode = std::get<1>(routeDetailParent2[v]);
                    endNode = std::get<2>(routeDetailParent2[v]);
                    jparent = startNode;

                    for (jparent = startNode; jparent < endNode; ++jparent) {
                        if (parent2[jparent] == 0) {
                            parentTemporary1[j] = parent2[jparent];
                            ++j;
                        }

                        if (servedCities[parent2[jparent]] == 0) {
                            parentTemporary1[j] = parent2[jparent];
                            servedCities[parent2[jparent]] = 1;
                            ++j;
                        }
                    }
                }
            }
            fillRouteDetail(distanceMatrix);
            routeDetailParentTemporary1 = routeDetailChecker;
            std::fill(notServedNode.begin(), notServedNode.end(), std::make_pair(-1, -1));
            nCities = 0;
            for (int cities = 0; cities < numCities; ++cities) {
                if (servedCities[cities] == 0) {
                    notServedNode[nCities].first = cities;
                    ++nCities;
                }
            }
            nCities = 0;
            while (notServedNode[nCities].first != -1) {
                int nodeA = notServedNode[nCities].first;
                std::vector<int> sortedNodes = getNodesSortedByProximity(nodeA, distanceMatrix);
                int nearestNode;
                int cities = 0, j;
                while (cities < numCities && notServedNode[nCities].second == -1)
                {
                    int v = -1, numRouteNearestNode = -1;
                    if (cities >= numCities) {
                        std::cerr << "Error: cities index (" << cities << ") out of range! Max index: " << numCities - 1 << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    if (sortedNodes[cities] == 0 && cities < numCities - 1) {
                        ++cities;
                    }
                    else if (sortedNodes[cities] != 0) {
                        nearestNode = sortedNodes[cities];
                        if (servedCities[nearestNode] == 1) {
                            j = 0;
                            while (parentTemporary1[j] != nearestNode) {
                                if (parentTemporary1[j] == 0) { v = v + 1; }
                                ++j;
                                if (j >= parentTemporary1.size()) {
                                    std::cerr << "Error: j index (" << j << ") out of range! Max index: " << parentTemporary1.size() - 1 << std::endl;
                                    exit(EXIT_FAILURE);
                                }
                            }
                            numRouteNearestNode = v;
                            if (numRouteNearestNode < 0 || numRouteNearestNode >= routeDetailParentTemporary1.size()) {
                                std::cerr << "Error: numRouteNearestNode (" << numRouteNearestNode << ") out of range! Max index: " << routeDetailParentTemporary1.size() - 1 << std::endl;
                                exit(EXIT_FAILURE);
                            }
                            int availableCapacity = vehicleCapacity - std::get<3>(routeDetailParentTemporary1[numRouteNearestNode]);
                            if (nodeA >= demand.size()) {
                                std::cerr << "Error: nodeA (" << nodeA << ") out of range! Max index: " << demand.size() - 1 << std::endl;
                                exit(EXIT_FAILURE);
                            }
                            if (cities == numCities - 1) { break; }
                            if (demand[nodeA].second <= availableCapacity) {
                                if (nCities >= notServedNode.size()) {
                                    std::cerr << "Error: nCities (" << nCities << ") out of range! Max index: " << notServedNode.size() - 1 << std::endl;
                                    exit(EXIT_FAILURE);
                                }
                                notServedNode[nCities].second = nearestNode;
                                servedCities[nearestNode] = 1;
                                std::get<3>(routeDetailParentTemporary1[numRouteNearestNode]) = std::get<3>(routeDetailParentTemporary1[numRouteNearestNode]) + demand[nodeA].second;
                                jparent = 0;
                                for (int j = 0; j < chromosomeSize; ++j)
                                {
                                    if (parentTemporary1[jparent] == nearestNode)
                                    {
                                        parentTemporary2[j] = nearestNode;
                                        ++j;
                                        parentTemporary2[j] = nodeA;
                                        ++j; ++jparent;
                                    }
                                    parentTemporary2[j] = parentTemporary1[jparent];
                                    ++jparent;
                                }
                                parentTemporary1 = parentTemporary2;
                            }
                            else {
                                if (cities < numCities - 1) {
                                    ++cities;
                                }
                                else { break; }

                            }
                        }
                        else {
                            if (cities < numCities - 1) {
                                ++cities;
                            }
                            else { break; }
                        }
                    }
                    else {
                        break;
                    }
                    if (cities == numCities - 1) { break; }
                }
                ++nCities;
            }
            checker = parentTemporary1;
            fillRouteDetail(distanceMatrix);
            routeDetailParentTemporary1 = routeDetailChecker;
            parentTemporary1 = checker;
            allNodesServedChecker();
            routeDetailChecker = routeDetailParentTemporary1;
            capacityConstraintChecker();
            if (servedCount == numCities && capacityChecker == 1) {
                processSuccessLabel[selectparent2] = 1;
                newPopulation[selectparent2].route = parentTemporary1;
                std::copy(parentTemporary1.begin(), parentTemporary1.end(), newPopulation[selectparent2].route.begin());
                for (int v = 0; v < numVehicle; ++v) {
                    int feas = std::get<0>(routeDetailParentTemporary1[v]);
                    int sNode = std::get<1>(routeDetailParentTemporary1[v]);
                    int eNode = std::get<2>(routeDetailParentTemporary1[v]);
                    int cLoad = std::get<3>(routeDetailParentTemporary1[v]);
                    double tDist = std::get<4>(routeDetailParentTemporary1[v]);
                    newPopulationDetail[selectparent2][v] = IndividuDetail(feas, sNode, eNode, cLoad, tDist);
                }
            }
            else {


                processSuccessLabel[selectparent2] = -1;
                newPopulation[selectparent2] = population[selectparent2];
                newPopulationDetail[selectparent2] = individuDetail[selectparent2];
            }
        }
        else if (selectparent1 != -1)
        {
            newPopulation[selectparent1] = population[selectparent1];
            newPopulationDetail[selectparent1] = individuDetail[selectparent1];
            processSuccessLabel[selectparent1] = 0;
        }
        else {
            break;
        }
        selectparent1 = -1, selectparent2 = -1;
    }
    calculateFitness(newPopulation, distanceMatrix);
}

void constrainedTailExchangeMutation() {
    for (int i = 0; i < newPopulation.size(); ++i) {
        newPopulation[i] = Individual();
        std::fill(newPopulation[i].route.begin(), newPopulation[i].route.end(), 0);
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, 1.0);
    std::vector<double> randomMutation(populationSize);
    for (int i = 0; i < populationSize; ++i) {
        randomMutation[i] = distrib(gen);
    }
    std::vector<int> individuMutation(chromosomeSize, 0);
    std::vector<int> individuMutationTemporary(chromosomeSize, 0);
    std::vector<std::tuple< int, int, int, int, double>> routeDetailIndividuMutation(numVehicle, { 0, 0, 0, 0, 0 });
    std::vector<std::tuple< int, int, int, int, double>> routeDetailIndividuMutationTemporary(numVehicle, { 0, 0, 0, 0, 0 });


    for (int i = 0; i < populationSize; ++i)
    {
        if (randomMutation[i] <= mutationRate) {
            individuMutation = population[i].route;
            for (int v = 0; v < numVehicle; ++v) {
                routeDetailIndividuMutation[v] = std::make_tuple(
                    individuDetail[i][v].feasibility,
                    individuDetail[i][v].startNode,
                    individuDetail[i][v].endNode,
                    individuDetail[i][v].currentLoad,
                    individuDetail[i][v].travelDistance
                );
            }
            std::uniform_int_distribution<> distrib(0, numVehicle - 1);
            int selectedRouteMutation1 = distrib(gen);
            int selectedRouteMutation2;
            do { selectedRouteMutation2 = distrib(gen); } while (selectedRouteMutation2 == selectedRouteMutation1);
            int numGeneMutation = chromosomeSize / 100;
            int startNodeMutation1 = std::get<1>(routeDetailIndividuMutation[selectedRouteMutation1]);
            int endNodeMutation1 = std::get<2>(routeDetailIndividuMutation[selectedRouteMutation1]);
            int startNodeMutation2 = std::get<1>(routeDetailIndividuMutation[selectedRouteMutation2]);
            int endNodeMutation2 = std::get<2>(routeDetailIndividuMutation[selectedRouteMutation2]);
            if (endNodeMutation1 - (startNodeMutation1 + 1) < numGeneMutation) {
                numGeneMutation = endNodeMutation1 - (startNodeMutation1 + 1);
            }
            if (endNodeMutation2 - (startNodeMutation2 + 1) < numGeneMutation) {
                numGeneMutation = endNodeMutation2 - (startNodeMutation2 + 1);
            }
            int jOtherRoute = endNodeMutation2 - numGeneMutation;
            for (int j = endNodeMutation1 - numGeneMutation; j < endNodeMutation1; ++j) {
                individuMutation[j] = population[i].route[jOtherRoute];
                ++jOtherRoute;
            }
            jOtherRoute = endNodeMutation1 - numGeneMutation;
            for (int j = endNodeMutation2 - numGeneMutation; j < endNodeMutation2; ++j) {
                individuMutation[j] = population[i].route[jOtherRoute];
                ++jOtherRoute;
            }
            checker = individuMutation;
            fillRouteDetail(distanceMatrix);
            routeDetailIndividuMutation = routeDetailChecker;
            int currentLoad = std::get<3>(routeDetailIndividuMutation[selectedRouteMutation1]);
            if (currentLoad < std::get<3>(routeDetailIndividuMutation[selectedRouteMutation2])) {
                currentLoad = std::get<3>(routeDetailIndividuMutation[selectedRouteMutation2]);
            }
            if (currentLoad > vehicleCapacity) {
                newPopulation[i] = population[i];
                newPopulationDetail[i] = individuDetail[i];
                processSuccessLabel[i] = -2;
            }
            else {
                newPopulation[i].route = individuMutation;
                processSuccessLabel[i] = 2;
                for (int v = 0; v < numVehicle; ++v) {
                    int feas = std::get<0>(routeDetailIndividuMutation[v]);
                    int sNode = std::get<1>(routeDetailIndividuMutation[v]);
                    int eNode = std::get<2>(routeDetailIndividuMutation[v]);
                    int cLoad = std::get<3>(routeDetailIndividuMutation[v]);
                    double tDist = std::get<4>(routeDetailIndividuMutation[v]);
                    newPopulationDetail[i][v] = IndividuDetail(feas, sNode, eNode, cLoad, tDist);
                }
            }
        }
        else {
            newPopulation[i] = population[i];
            newPopulationDetail[i] = individuDetail[i];
            processSuccessLabel[i] = 10;
        }
    }
    calculateFitness(newPopulation, distanceMatrix);
}

void breederElitism() {
    std::vector<Individual> sortedPopulation(populationSize);
    std::copy(newPopulation.begin(), newPopulation.end(), sortedPopulation.begin());
    std::sort(sortedPopulation.begin(), sortedPopulation.end(),
        [](const Individual& a, const Individual& b) {
            return a.fitness > b.fitness;
        });
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, 1.0);
    std::vector<double> randomBreder(populationSize);
    for (int i = 0; i < populationSize; ++i) {
        randomBreder[i] = distrib(gen);
    }
    int numSortedPopulation = 0;
    for (int i = 0; i < populationSize; ++i) {
        if (randomBreder[i] <= breederRate) {
            newPopulation[i] = sortedPopulation[numSortedPopulation];
            ++numSortedPopulation;
        }
        else {
            newPopulation[i] = population[i];
        }
    }
}

std::string formatDouble(double value, int precision = 2) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

int main() {

    int gen = 0;
    std::cout << "populationSize: " << populationSize << std::endl;
    std::cout << "numCities: " << numCities << std::endl;
    std::cout << "generations: " << generations << std::endl;
    std::cout << "vehicleCapacity: " << vehicleCapacity << std::endl;
    std::cout << "numVehicle: " << numVehicle << std::endl;
    std::cout << "chromosomeSize: " << chromosomeSize << std::endl;

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    char buffer[26];
    ctime_s(buffer, sizeof(buffer), &now_time);

    std::string path2 = std::string("d:\\FoG(CAKNN5)=")
        + benchmarkID
        + "_PopS=" + std::to_string(populationSize)
        + "_Gens=" + std::to_string(generations)
        + "_CR=" + formatDouble(crossoverRate)
        + "_MR=" + formatDouble(mutationRate)
        + "_BR=" + formatDouble(breederRate)
        + ".csv";
    std::ofstream MyExcelFile2(path2);
    MyExcelFile2 << buffer;
    MyExcelFile2 << "\n";
    MyExcelFile2 << "crossover rate : " << crossoverRate << "," << "mutation rate : " << mutationRate << "," << "breeder rate : " << breederRate << "\n";
    distanceMatrix = calculateDistanceMatrix(coordinates, benchmarkID);
    geneticAlgorithm(populationSize, chromosomeSize, numCities, numVehicle, generations, gen, coordinates, demand, vehicleCapacity, benchmarkID, distanceMatrix, MyExcelFile2);
    std::cout << "Result has been saved on drive D:" << std::endl;
    return 0;
}

void geneticAlgorithm(int populationSize, int chromosomeSize, int numCities, int numVehicle, int generations, int gen, const std::vector<std::pair<int, int>>& coordinates, const std::vector<std::pair<int, int>>& demand, int vehicleCapacity, const std::string& benchmarkID, const std::vector<std::vector<double>>& distanceMatrix, std::ofstream& MyExcelFile2) {
    for (int i = 0; i < populationSize; ++i) {
        bestOverall[i].route = std::vector<int>(chromosomeSize, 0);
        bestOverall[i].fitness = 0.0;
        bestOverall[i].totalTravelDistance = std::numeric_limits<double>::infinity();
    }

    for (int igen2 = 0; igen2 < run; ++igen2) {
        GEN2 = igen2;
        bestOverall[GEN2].totalTravelDistance = std::numeric_limits<double>::infinity();
        std::cout << "\n\nGenerating Initial Population for Run " << igen2 + 1 << std::endl;
        population = initializePopulation(populationSize, numCities, numVehicle, vehicleCapacity, demand, coordinates, benchmarkID, distanceMatrix);
        calculateFitness(population, distanceMatrix);
        findBestIndividual(population);
        geneticDiversity = calculateGeneticDiversity(population);


        MyExcelFile2 << "Starting of Run " << igen2 + 1 << "\n";
        MyExcelFile2 << "Initial Population for run " << igen2 + 1 << "\n";
        gen = igen2;

        MyExcelFile2 << "Generation" << ",";
        for (int i = 0; i < chromosomeSize; ++i) {
            MyExcelFile2 << "Gene " << i + 1 << ",";
        }
        MyExcelFile2 << "Fitness Value" << "," << "Objective Value (distance)" << "\n";
        writePopulationToCSV(gen, MyExcelFile2, population);
        std::fill(newPopulation.begin(), newPopulation.end(), Individual());
        MyExcelFile2 << "Objective Value (distance) obtained after GA process over generations" << "\n";
        MyExcelFile2 << "Generation" << ",";
        for (int i = 0; i < populationSize; ++i) {
            MyExcelFile2 << "Individual " << i + 1 << ",";
        }
        MyExcelFile2 << "genetic Diversity" << "," << "best Overall" << "\n";

        for (gen = 0; gen < generations; ++gen) {
            std::cout << "Roulette Wheel Selection\n";
            for (int i = 0; i < populationSize; ++i) {
                Individual selectedIndividual = rouletteWheelSelection(population);
                newPopulation[i] = selectedIndividual;
            }
            population = newPopulation;
            findBestIndividual(population);
            fillIndividuDetail();
            std::cout << "Crossover\n";
            proximityConstrainedSegmentCrossover();
            population = newPopulation;
            findBestIndividual(population);
            fillIndividuDetail();
            std::cout << "Mutation\n";
            constrainedTailExchangeMutation();
            population = newPopulation;
            findBestIndividual(population);
            fillIndividuDetail();

            std::cout << "Breeder Elitism\n";
            breederElitism();
            population = newPopulation;
            fillIndividuDetail();
            findBestIndividual(population);
            geneticDiversity = calculateGeneticDiversity(population);
            writeFitnessToCSV(gen, MyExcelFile2, population);
            std::cout << "-------------Generation " << gen + 1 << ", Run " << GEN2 + 1 << "-------------" << std::endl;
        }
        MyExcelFile2 << "\nIndividuals obtained at the last generation\n";
        MyExcelFile2 << "Generation" << ",";
        for (int i = 0; i < populationSize; ++i) {
            MyExcelFile2 << "Individual " << i + 1 << ",";
        }
        MyExcelFile2 << "genetic Diversity" << "," << "best Overall" << "\n";
        writePopulationToCSV(gen, MyExcelFile2, population);
        MyExcelFile2 << "best individual obtained overall run " << igen2 + 1 << "\n";
        for (int i = 0; i < chromosomeSize; ++i) {
            MyExcelFile2 << "Gene " << i + 1 << ",";
        }
        MyExcelFile2 << "Fitness Value" << "," << "Objective Value (distance)" << "\n";
        for (size_t j = 0; j < bestOverall[GEN2].route.size(); ++j) {
            MyExcelFile2 << bestOverall[GEN2].route[j] << ",";
        }
        MyExcelFile2 << bestOverall[GEN2].fitness << ",";
        MyExcelFile2 << bestOverall[GEN2].totalTravelDistance << "\n";
        MyExcelFile2 << "\n";
        MyExcelFile2 << "Ending of Run " << igen2 + 1 << "\n";

        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        char buffer[26];
        ctime_s(buffer, sizeof(buffer), &now_time);
        MyExcelFile2 << buffer;
        MyExcelFile2 << "\n";
    }
}

double calculateDistance(const std::pair<int, int>& point1, const std::pair<int, int>& point2) {
    int dx = point2.first - point1.first;
    int dy = point2.second - point1.second;
    return std::sqrt(dx * dx + dy * dy);
}

std::vector<std::vector<double>> calculateDistanceMatrix(const std::vector<std::pair<int, int>>& coordinates, const std::string& benchmarkID) {
    int numCities = coordinates.size();
    for (int i = 0; i < numCities; ++i) {
        for (int j = 0; j < numCities; ++j) {
            if (i != j) {
                distanceMatrix[i][j] = calculateDistance(coordinates[i], coordinates[j]);
            }
            else {
                distanceMatrix[i][j] = 0.0;
            }
        }
    }    
    return distanceMatrix;
}

int generateRandomNumber(int numCities, const std::set<int>& forbiddenNumbers) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, numCities - 1);
    int randomNumber;
    do {
        randomNumber = distrib(gen);
    } while (forbiddenNumbers.find(randomNumber) != forbiddenNumbers.end());
    return randomNumber;
}

void printOnlyServedCities(const std::vector<int>& servedCities) {
    std::cout << "Served Cities Indexes: \n";
    for (size_t sc = 0; sc < servedCities.size(); ++sc) {
        if (servedCities[sc] == 1) {
            std::cout << sc << " ";
        }
    }
    std::cout << std::endl;
}

void printOnlyNotServedCities(const std::vector<int>& servedCities, const std::vector<std::pair<int, int>>& demanddescendingsort) {
    std::cout << "Not Served Cities Indexes: \n";

    int minDemand = INT_MAX;
    int minDemandCityIndex = -1;

    for (size_t sc = 0; sc < servedCities.size(); ++sc) {
        if (servedCities[sc] == 0) {
            std::cout << "City Index: " << sc << ", Demand: " << demanddescendingsort[sc].second << std::endl;

            if (demanddescendingsort[sc].second < minDemand) {
                minDemand = demanddescendingsort[sc].second;
                minDemandCityIndex = sc;
            }
        }
    }

    if (minDemandCityIndex != -1) {
        std::cout << "\nCity with the lowest demand: City Index: " << minDemandCityIndex
            << ", Demand: " << minDemand << std::endl;
    }
    else {
        std::cout << "\nNo not served cities found!" << std::endl;
    }
}

bool allCitiesServed(const std::vector<int>& servedCities, int numCities) {
    int count = std::count(servedCities.begin(), servedCities.end(), 1);
    return count == numCities;
}


bool hasUnservedCities(const std::vector<int>& servedCities) {
    return std::find(servedCities.begin(), servedCities.end(), 0) != servedCities.end();
}

int countServedCities(const std::vector<int>& servedCities) {
    return std::count(servedCities.begin(), servedCities.end(), 1);
}

void sortCitiesByDemand(vector<pair<int, int>>& cities) {
    sort(cities.begin(), cities.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second > b.second;
        });
}

void writePopulationToCSV(int gen, std::ofstream& file, const std::vector<Individual>& population) {
    if (!file.is_open()) {
        std::cerr << "Error: File CSV tidak terbuka!\n";
        return;
    }

    for (size_t i = 0; i < population.size(); ++i) {
        const auto& ind = population[i];
        file << gen << ",";

        for (size_t j = 0; j < ind.route.size(); ++j) {
            file << ind.route[j] << ",";
        }
        file << ind.fitness << ",";
        file << ind.totalTravelDistance << "\n";
    }
    file << "\n";
}

void writeFitnessToCSV(int gen, std::ofstream& file, const std::vector<Individual>& population) {
    if (!file.is_open()) {
        std::cerr << "Error: File CSV tidak terbuka!\n";
        return;
    }

    file << gen + 1 << ",";
    for (int i = 0; i < populationSize; ++i) {
        file << population[i].totalTravelDistance << ",";
    }
    file << totalhammingDistance << "," << bestOverall[GEN2].totalTravelDistance << ",";
    file << "\n";
}

void writeIndividuDetailToCSV(std::ofstream& file, const std::vector<std::vector<IndividuDetail>>& individuDetail) {

    if (!file.is_open()) {
        std::cerr << "Error: Gagal membuka file CSV!\n";
        return;
    }

    file << "Individu,";
    for (int v = 0; v < numVehicle; ++v) {
        file << "Route, Feasibility, StartNode, EndNode, CurrentLoad, TravelDistance,";
    }
    file << "\n";

    for (int i = 0; i < populationSize; ++i) {
        file << i << ",";
        for (int v = 0; v < numVehicle; ++v) {
            file << v << ","
                << individuDetail[i][v].feasibility << ","
                << individuDetail[i][v].startNode << ","
                << individuDetail[i][v].endNode << ","
                << individuDetail[i][v].currentLoad << ","
                << individuDetail[i][v].travelDistance << ",";
        }
        file << "\n";
    }
    file << "\n";

}

std::vector<int> getNodesSortedByProximity(int nodeA, const std::vector<std::vector<double>>& distanceMatrix) {
    int numCities = distanceMatrix.size();
    std::vector<std::pair<double, int>> distanceToNodes;

    for (int i = 0; i < numCities; ++i) {
        if (i != nodeA) {
            distanceToNodes.push_back({ distanceMatrix[nodeA][i], i });
        }
    }
    std::sort(distanceToNodes.begin(), distanceToNodes.end());
    std::vector<int> sortedNodes;
    for (const auto& pair : distanceToNodes) {
        sortedNodes.push_back(pair.second);
    }
    return sortedNodes;
}

void allNodesServedChecker() {
    for (int j = 0; j < chromosomeSize; ++j) {
        servedCities[checker[j]] = 1;
    }
    servedCount = countServedCities(servedCities);
    return;
}

void capacityConstraintChecker() {
    int feasibility = 1;
    for (int v = 0; v < numVehicle; ++v) {
        if (std::get<0>(routeDetailChecker[v]) < feasibility) {
            feasibility = std::get<0>(routeDetailChecker[v]);
        }
    }
    if (feasibility == 1) {
        capacityChecker = 1;
    }
    else { capacityChecker = 0; }

    return;
}

void fillRouteDetail(const std::vector<std::vector<double>>& distanceMatrix) {
    int feasibility = 0, currentLoad = 0, startNode = 0, endNode = 0;
    double travelDistance = 0.0;
    int j = 0, node;
    if (routeDetailChecker.size() < numVehicle) {
        routeDetailChecker.resize(numVehicle);
    }
    for (int v = 0; v < numVehicle; ++v) {
        currentLoad = 0;
        travelDistance = 0.0;
        startNode = j;
        if (j >= checker.size()) {
            std::cerr << "Error: j (" << j << ") melebihi ukuran checker (" << checker.size() << ") saat inisialisasi!" << std::endl;
            break;
        }
        ++j;

        if (j >= checker.size()) {
            std::cerr << "Error: j (" << j << ") melebihi ukuran checker (" << checker.size() << ") setelah inkrementasi!" << std::endl;
            break;
        }
        node = checker[j];
        while (j < checker.size() && node != 0) {
            if (node >= demand.size()) {
                std::cerr << "Error: node (" << node << ") melebihi ukuran demand (" << demand.size() << ")!" << std::endl;
                break;
            }
            currentLoad += demand[node].second;
            if (j > 0 && j < checker.size()) {
                if (checker[j - 1] >= distanceMatrix.size() || checker[j] >= distanceMatrix[checker[j - 1]].size()) {
                    std::cerr << "Error: Indeks distanceMatrix out of range! checker[" << j - 1 << "] = " << checker[j - 1]
                        << ", checker[" << j << "] = " << checker[j] << std::endl;
                    break;
                }
                travelDistance += distanceMatrix[checker[j - 1]][checker[j]];
            }

            ++j;
            if (j >= checker.size()) {
                std::cerr << "Error: j (" << j << ") melebihi ukuran checker (" << checker.size() << ") dalam loop!" << std::endl;
                break;
            }
            node = checker[j];
        }

        feasibility = (currentLoad <= vehicleCapacity) ? 1 : 0;
        if (j > 0 && j < checker.size()) {
            if (checker[j - 1] >= distanceMatrix.size() || checker[j] >= distanceMatrix[checker[j - 1]].size()) {
                std::cerr << "Error: Indeks distanceMatrix out of range saat akhir loop!" << std::endl;
            }
            else {
                travelDistance += distanceMatrix[checker[j - 1]][checker[j]];
            }
        }
        endNode = j;
        if (v < routeDetailChecker.size()) {
            routeDetailChecker[v] = std::make_tuple(feasibility, startNode, endNode, currentLoad, travelDistance);
        }
        else {
            std::cerr << "Error: v (" << v << ") melebihi ukuran routeDetailChecker (" << routeDetailChecker.size() << ")!" << std::endl;
        }
    }
}

void fillIndividuDetail() {
    for (int i = 0; i < populationSize; ++i) {
        int feasibility, currentLoad, startNode, endNode;
        double travelDistance;
        int j = 0, node;

        for (int v = 0; v < numVehicle; ++v) {
            currentLoad = 0;
            travelDistance = 0.0;
            startNode = j;
            j = j + 1;

            if (j >= population[i].route.size()) {
                std::cerr << "Invalid j index: " << j << " for route size " << population[i].route.size() << "\n";
                break;
            }

            node = population[i].route[j];
            while (node != 0) {
                if (node < 0 || node >= demand.size()) {
                    std::cerr << "Invalid node: " << node << "\n";
                    break;
                }
                currentLoad = currentLoad + demand[node].second;
                if (population[i].route[j - 1] < 0 || population[i].route[j] < 0 || population[i].route[j - 1] >= distanceMatrix.size() || population[i].route[j] >= distanceMatrix.size()) {
                    std::cerr << "Invalid route indices: " << population[i].route[j - 1] << " and " << population[i].route[j] << "\n";
                    break;
                }
                travelDistance = travelDistance + distanceMatrix[population[i].route[j - 1]][population[i].route[j]];
                ++j;
                if (j >= population[i].route.size()) {
                    std::cerr << "Index j out of bounds: " << j << "\n";
                    break;
                }
                node = population[i].route[j];
            }

            if (currentLoad <= vehicleCapacity) { feasibility = 1; }
            else { feasibility = 0; }

            endNode = j;
            travelDistance = travelDistance + distanceMatrix[population[i].route[j - 1]][population[i].route[j]];

            individuDetail[i][v] = IndividuDetail(feasibility, startNode, endNode, currentLoad, travelDistance);
        }
    }

}

void findBestIndividual(const std::vector<Individual>& population) {
    for (int i = 0; i < populationSize; ++i) {
        if (population[i].totalTravelDistance < bestOverall[GEN2].totalTravelDistance) {
            bestOverall[GEN2] = population[i];
        }
    }


}

double calculateDistance(const vector<int>& route, const vector<vector<double>>& distanceMatrix) {
    double totalDistance = 0.0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        totalDistance += distanceMatrix[route[i]][route[i + 1]];
    }
    return totalDistance;
}

int calculateHammingDistance(const std::vector<int>& a, const std::vector<int>& b) {
    hammingDistance = 0;
    int size = a.size();
    for (int i = 0; i < size; ++i) {
        if (a[i] != b[i]) {
            hammingDistance++;
        }
    }
    return hammingDistance;
}

double calculateGeneticDiversity(const std::vector<Individual>& population) {

    totalhammingDistance = 0;
    int numPairs = 0;

    for (int i = 0; i < populationSize; ++i) {
        for (int j = i + 1; j < populationSize; ++j) {
            int hdistance = calculateHammingDistance(population[i].route, population[j].route);
            totalhammingDistance += hdistance;
            numPairs++;
        }
    }
    return static_cast<double>(totalhammingDistance) / numPairs;
}

int countVisitedCities(const Individual& individual) {
    std::unordered_set<int> visitedCities;
    for (int city : individual.route) {
        visitedCities.insert(city);
    }
    return visitedCities.size();
}






