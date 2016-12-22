#pragma once
#include<vector>
#include<unordered_map>
#include <functional>

template<class Class, typename Type, Type Class::*PtrToMember>
struct non_const_member_base
{
    typedef Type result_type;
    const Type& operator()(const Class& x)const
    {
        return x.*PtrToMember;
    }

    Type& operator()(Class& x)const
    {
        return x.*PtrToMember;
    }
};

template<class Class, class Cmpp, typename KeyType, KeyType Class::*PtrToMember>
class RankTemplate
{
    //typedef non_const_member_base<Class, KeyType, Class::*PtrToMember> member_base;
public:
    ~RankTemplate()
    {
        KeyRanks.clear();
        for (int i = 0; i < (int)Ranks.size(); i++)
        {
            if (cbDelete != nullptr)
                cbDelete(Ranks[i]);
            KeyType key = GetKeyValue(*Ranks[i]);
            delete Ranks[i];
        }
        Ranks.clear();
    }
    void OnDataChange(int max_size, Class& data)
    {
        KeyType key = GetKeyValue(data);
        std::unordered_map<KeyType, Class*>::iterator keyit = KeyRanks.find(key);
        if (keyit != KeyRanks.end())
        {
            //如果在排行榜里找到玩家 就更新玩家
            //item.dbid = playerit->second->dbid;

            Class before = *keyit->second;
            *keyit->second = data;

            if (cbUpdate != nullptr)
                cbUpdate(keyit->second, &before);
        }
        else
        {
            //如果没有找到玩家 并且符合排行条件  就进入排行榜
            Class* pmItem = new Class;
            *pmItem = data;
            if (cbInsert != nullptr)
                cbInsert(pmItem);

            Ranks.push_back(pmItem);

            key = GetKeyValue(data);
            KeyRanks.insert(std::make_pair(key, pmItem));
        }

        std::sort(Ranks.begin(), Ranks.end(), Cmpp());

        //剔除多余的玩家
        if ((int)Ranks.size() > max_size)
        {
            for (int i = max_size; i < (int)Ranks.size(); i++)
            {
                if (cbDelete != nullptr)
                    cbDelete(Ranks[i]);
                KeyType key = GetKeyValue(*Ranks[i]);
                delete Ranks[i];
                KeyRanks.erase(key);
            }
            Ranks.erase(Ranks.begin() + max_size, Ranks.end());
        }
    }

private:
    KeyType& GetKeyValue(Class& data)
    {
        return member_base(data);
    }
    void SetKeyValue(Class& dest, Class& src)
    {
        GetKeyValue(dest) = GetKeyValue(src);
    }
    std::vector<Class*> Ranks;
    std::unordered_map<KeyType, Class*> KeyRanks;
    non_const_member_base<Class, KeyType, PtrToMember> member_base;   

    std::function<void(Class*)> cbInsert;
    std::function<void(Class*, Class*)> cbUpdate; 
    std::function<void(Class*)> cbDelete;
};