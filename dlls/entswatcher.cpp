#include	"extdll.h"
#include	"util.h"
#include "entswatcher.h"
#include "cbase.h"
#include <vector>
#include <algorithm>
#include <iterator>

namespace 
{
    struct ent_desc_t
    {
        explicit ent_desc_t(EHANDLE handle)
            : handle(handle)
            , classname(STRING(handle->pev->classname))
        {
        }

        ~ent_desc_t()
        {
            int aaa = 5;
        }

        void update()
        {
            classname = STRING(handle->pev->classname);
        }

        EHANDLE handle;
        char const *classname;
    };

    typedef std::shared_ptr<ent_desc_t> ent_desc_ptr;
} // namespace 


struct EntsWatcherImpl : EntsWatcher
{
    void AddEnt(CBaseEntity* ent) override
    {
        // pending_.push_back(std::make_pair(ent, &typeid(*ent)));

        auto const classname = STRING(ent->pev->classname);

        EHANDLE h;
        h = ent;
        ents_.push_back(std::make_shared<ent_desc_t>(h));
    }

    void Update() override
    {
        auto const gone = RemoveDead();
        
        static std::vector<char const*> names;
        names.clear();
        
        std::transform(ents_.begin(), ents_.end(), std::back_inserter(names),
            [](ent_desc_ptr ptr)
        {
            return STRING(ptr->handle.Get()->v.classname);
        });

        int aaa = 5;

        for (auto it = ents_.begin(); it != ents_.end(); ++it)
            (*it)->update();
    }


    void Reset() override
    {
        *this = EntsWatcherImpl();
    }
private:
    size_t RemoveDead()
    {
        auto is_dead = [](ent_desc_ptr &ptr) -> bool
        {
            bool const ret = !UTIL_IsValidEntity(ptr->handle.Get());
                        
            return ret;
        };
        
        
        auto it = std::remove_if(ents_.begin(), ents_.end(), is_dead);

        auto const gone = ents_.end() - it;

        ents_.erase(it, ents_.end());

        return gone;
    }
private:
    std::vector<ent_desc_ptr> ents_;
    std::vector<std::pair<CBaseEntity *, std::type_info const*>> pending_;
};

EntsWatcher& EntsWatcher::Instance()
{
    static EntsWatcherImpl impl;
    return impl;
}
