/*
 * randomNumGen.cpp
 *
 *  Created on: 2018年7月28日
 *      Author: mytest
 */

#include "randomNumGen.h"
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <cctype>
#include <string>
#include <map>
randomNumGenerator *randomNumGenerator::rGen = nullptr;
std::string randomNumGenerator::randomGenName = "";

randomNumGenerator * randomNumGenerator::getConcretRandomGen(std::string& rGenName)
{
    if(rGen)
        return rGen;
    else
    {
        randomGenName = rGenName;
        if(randomGenName == "uniform")
        {
            rGen = new uniformDistribution;

            return rGen;
        }
        else if(randomGenName == "bernoulli")
        {
            rGen = new bernoulliDistribution(0.25);
            return rGen;
        }
        else
        {
            std::cerr << "Unknown random number generator." << std::endl;
            throw std::runtime_error("");
        }
    }
}

size_t bernoulliDistribution::getUint(size_t lowerLimit, size_t upperLimit) const
{
    bool x = (lowerLimit <= upperLimit);
    if(!x)
    {
        std::cerr << "lowerLimit must be a number less than upperLimit." << std::endl;
        throw std::runtime_error("");
    }
    if(lowerLimit == upperLimit)
        return lowerLimit;
    size_t base = lowerLimit;
    size_t offset = upperLimit - lowerLimit;
    std::map<bool, int> imap;
    imap[0] = 0;
    imap[1] = 0;
    for(size_t i = 0; i < offset; ++i)
        ++imap[(*dist)(*gen)];
    return (base + imap[1]);
}

double bernoulliDistribution::getFloat(double lowerLimit, double upperLimit) const
{
    bool x = (lowerLimit <= upperLimit);
    if(!x)
    {
        std::cerr << "lowerLimit must be a number less than upperLimit." << std::endl;
        throw std::runtime_error("");
    }
    size_t l = static_cast<size_t>(lowerLimit * 100);
    size_t h = static_cast<size_t>(upperLimit * 100);
    bool _x = ((lowerLimit / l) == 1.0);
    bool _y = ((upperLimit / h) == 1.0);
    if(_x && _y)
        return this->getUint(l, h);
    else
    {
        std::cerr << "the number of decimal places is up to 2." << std::endl;
        throw std::runtime_error("");
    }
}

size_t uniformDistribution::getUint(size_t lowerLimit, size_t upperLimit) const
{
    bool x = (lowerLimit <= upperLimit);
    if(!x)
    {
        std::cerr << "lowerLimit must be a number less than upperLimit." << std::endl;
        throw std::runtime_error("");
    }
    std::uniform_int_distribution<size_t> dist(lowerLimit, upperLimit);
    return dist(*gen);
}

double uniformDistribution::getFloat(double lowerLimit, double upperLimit) const
{
    assert(lowerLimit <= upperLimit);
    bool x = (lowerLimit <= upperLimit);
    if(!x)
    {
        std::cerr << "lowerLimit must be a number less than upperLimit." << std::endl;
        throw std::runtime_error("");
    }
    std::uniform_real_distribution<double> dist(lowerLimit, upperLimit);
    return dist(*gen);
}

randomNumGenerator::~randomNumGenerator()
{
    delete this->rGen;
}
