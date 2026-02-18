#pragma once
#include "Debug.hpp"
#include <string>
#include <vector>
#include <memory>
#include <array>

template<class T,class... Args>
using vector = std::vector<T>;

using string = std::string;

template<class T>
using pShared = std::shared_ptr<T>;

template<class T>
using pUnique = std::unique_ptr<T>;

template<class T>
using pWeak = std::weak_ptr<T>;
