#pragma once
#include "Debug.hpp"
#include <string>
#include <vector>
#include <memory>

template<class T>
using vector = std::vector<T>;

template<class T>
using pShared = std::shared_ptr<T>;

template<class T>
using pUnique = std::unique_ptr<T>;