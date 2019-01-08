#include	"extdll.h"
#include	"util.h"
#include "entswatcher.h"
#include "cbase.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>

namespace 
{
    struct ent_desc_t
    {
        explicit ent_desc_t(EHANDLE handle)
            : handle(handle)
            , classname(STRING(handle->pev->classname))
        {
            int aaa = 5;
        }

        ~ent_desc_t()
        {
            int aaa = 5;
        }

        void update()
        {
            const auto model = handle->pev->model;
            if (model != old_model_)
            {
                if (old_model_)
                {
                    auto const test = STRING(model);
                    int aaa = 5;
                }
                
                old_model_ = model;
            }
        }
    
 
        EHANDLE handle;
        char const *classname;
    private:
        string_t old_model_ = 0;
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
        pending_.push_back(h);
    }

    void Update() override
    {
        auto const gone = RemoveDead();
        AddPending();

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

    void AddPending()
    {
        for (auto &h : pending_)
        {
            if (UTIL_IsValidEntity(h.Get()))
                ents_.push_back(std::make_shared<ent_desc_t>(h));
        }

        pending_.clear();
    }

private:
    std::vector<ent_desc_ptr> ents_;
    std::vector<EHANDLE> pending_;
};

EntsWatcher& EntsWatcher::Instance()
{
    static EntsWatcherImpl impl;
    return impl;
}
