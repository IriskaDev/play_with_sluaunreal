#pragma once

#include "SluaMicro.h"
#include "UObject/UnrealType.h"
#include "UObject/GCObject.h"
#include "PropertyUtil.h"
#include "lauxlib.h"

namespace NS_SLUA {

    // Traits class which determines whether or not a type is a TSet.
    template <typename T> struct TIsTSet { enum { Value = false }; };

    template <typename InElementType, typename KeyFuncs, typename Allocator>
    struct TIsTSet<               TSet<InElementType, KeyFuncs, Allocator>> { enum { Value = true }; };

    template <typename InElementType, typename KeyFuncs, typename Allocator>
    struct TIsTSet<const          TSet<InElementType, KeyFuncs, Allocator>> { enum { Value = true }; };

    template <typename InElementType, typename KeyFuncs, typename Allocator>
    struct TIsTSet<      volatile TSet<InElementType, KeyFuncs, Allocator>> { enum { Value = true }; };

    template <typename InElementType, typename KeyFuncs, typename Allocator>
    struct TIsTSet<const volatile TSet<InElementType, KeyFuncs, Allocator>> { enum { Value = true }; };

    class SLUA_UNREAL_API LuaSet : public FGCObject {

    public:
        LuaSet(FProperty* Property, FScriptSet* Buffer, bool bIsRef, bool bIsNewInner);
        LuaSet(FSetProperty* Property, FScriptSet* Buffer, bool bIsRef);
        ~LuaSet();
        
        static void reg(lua_State* L);
        static void clone(FScriptSet* DstSet, FProperty* Property, const FScriptSet* SrcSet);
        static int push(lua_State* L, FProperty* Property, FScriptSet* Set, bool bIsNewInner);
        static int push(lua_State* L, LuaSet* luaSet);

        template<typename T>
        static int push(lua_State* L, const TSet<T>& V)
        {
            FProperty* Property = PropertyProto::createDeduceProperty<T>();
            const FScriptSet* Set = reinterpret_cast<const FScriptSet*>(&V);
            return push(L, Property, const_cast<FScriptSet*>(Set), false);
        }

        // Otherwise the LuaSet is an abstract class and can't be created.
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

#if !((ENGINE_MINOR_VERSION<20) && (ENGINE_MAJOR_VERSION==4))
        virtual FString GetReferencerName() const override
        {
            return "LuaSet";
        }
#endif

        FScriptSet* get() const;

        template<typename T>
        const TSet<T>& asTSet(lua_State* L) const
        {
            if (sizeof(T) != InElementProperty->ElementSize)
                luaL_error(L, "Cast to TSet error, element size doesn't match (%d, %d)", sizeof(T), InElementProperty->ElementSize);
            return *(reinterpret_cast<const TSet<T>*>(Set));
        }
        
    protected:
        static int __ctor(lua_State* L);
        static int Num(lua_State* L);
        static int Get(lua_State* L);
        static int Add(lua_State* L);
        static int Remove(lua_State* L);
        static int Clear(lua_State* L);
        static int Pairs(lua_State* L);
        static int Enumerable(lua_State* L);
        static int CreateElementTypeObject(lua_State* L);

    private:
        FScriptSet* Set;
        FProperty* InElementProperty;
        FScriptSetHelper Helper;

        bool IsRef;
        bool isNewInner;

        struct Enumerator
        {
            LuaSet* Set = nullptr;
            int32 Index = 0;
            int32 Num = 0;
            static int gc(lua_State* L);
        };

        static int setupMT(lua_State* L);
        static int gc(lua_State* L);

        int32 num() const;
        void clear();
        void emptyElements(int32 Slack = 0);
        void removeAt(int32 Index, int32 Count = 1);
        bool removeElement(const void* ElementToRemove);
    };
}
