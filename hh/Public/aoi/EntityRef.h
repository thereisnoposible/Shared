#pragma once

namespace AOI
{
	class Entity;
	class MemoryStream;

#define ENTITYREF_FLAG_UNKONWN							0x00000000
#define ENTITYREF_FLAG_ENTER_CLIENT_PENDING				0x00000001	// 进入客户端中标志
#define ENTITYREF_FLAG_LEAVE_CLIENT_PENDING				0x00000002	// 离开客户端中标志
#define ENTITYREF_FLAG_NORMAL							0x00000004	// 常规状态

	class EntityRef
	{
	public:
		EntityRef(Entity* pEntity);
		EntityRef();

		~EntityRef();

		void flags(unsigned int  v){ flags_ = v; }
		void removeflags(unsigned int  v){ flags_ &= ~v; }
		unsigned int  flags(){ return flags_; }

		Entity* pEntity() const { return pEntity_; }
		void pEntity(Entity* e);

		int id() const{ return id_; }

	private:
		int id_;
		Entity* pEntity_;
		unsigned int flags_;
	};

}