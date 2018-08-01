/*
 * randomNumGen.h
 *
 *  Created on: 2018年7月28日
 *      Author: mytest
 */

#ifndef _UTILITIES_RANDOMNUMGEN_H
#define _UTILITIES_RANDOMNUMGEN_H
#include <string>
#include <random>

class randomNumGenerator
{
public:
    static randomNumGenerator* getConcretRandomGen(std::string&);
    virtual size_t getUint(size_t lowerLimit, size_t upperLimit) const = 0;
    virtual double getFloat(double lowerLimit, double upperLimit) const = 0;
    virtual ~randomNumGenerator();
protected:
    static std::string randomGenName;
    std::mt19937 *gen;
protected:
    randomNumGenerator():gen(new std::mt19937((new std::random_device)->operator ()())){}
    static randomNumGenerator *rGen;
};

class bernoulliDistribution : public randomNumGenerator
{
public:
    bernoulliDistribution(double P):dist(new std::bernoulli_distribution(P)){}
    virtual size_t getUint(size_t lowerLimit, size_t upperLimit) const override;
    virtual double getFloat(double lowerLimit, double upperLimit) const override;
private:
    std::bernoulli_distribution *dist;
};

class uniformDistribution : public randomNumGenerator
{
public:
    uniformDistribution(){}
    virtual size_t getUint(size_t lowerLimit, size_t upperLimit) const override;
    virtual double getFloat(double lowerLimit, double upperLimit) const override;
private:
};
#endif /* _UTILITIES_RANDOMNUMGEN_H */
