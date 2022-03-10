#ifndef SA_SCHEDULING_RESOURCES_FACTORY_H
#define SA_SCHEDULING_RESOURCES_FACTORY_H

#include "memory"
#include "map"
#include "utility"
#include "string"
#include "../Problem/Conditions.h"

namespace Factories {
    template<class T>
    class Factory {
    protected:

        template<typename... Types>
        class ICreator {
        public:
            virtual std::shared_ptr<T> create() = 0;
            virtual std::shared_ptr<T> create(Types... args) = 0;
        };

        template<class S, typename... Types>
        class Creator: public ICreator<Types...> {
        public:
            std::shared_ptr <T> create(Types... args) override {
                if constexpr(std::is_constructible_v <S, Types...>) {
                    return std::make_shared<S>(S(args...));
                }
                return nullptr;
            }

            std::shared_ptr<T> create() override {
                if constexpr (std::is_constructible_v <S>) {
                    return std::make_shared<S>(S());
                }
                return nullptr;
            }
        };

        template<typename... Types>
        using Registered = std::map<std::string, std::shared_ptr<ICreator<Types...>>>;


        template<typename... Types>
        static Registered<Types...> registered;

    public:
        std::shared_ptr<T> create (std::string type) {
            return registered<>[type] -> create();
        }

        template <typename... Types>
        std::shared_ptr<T> create (std::string type, Types... args) {
            return registered<Types...>[type] -> create(args...);
        }
    };
}


#endif //SA_SCHEDULING_RESOURCES_FACTORY_H
