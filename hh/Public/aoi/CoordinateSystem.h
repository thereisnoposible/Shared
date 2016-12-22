#pragma once
#include <list>

namespace AOI
{
	class CoordinateNode;
	class CoordinateSystem
	{
	public:
		CoordinateSystem();
		~CoordinateSystem();

		bool insert(CoordinateNode* pNode);

		/**
		���ڵ��list���Ƴ�
		*/
		bool remove(CoordinateNode* pNode);
		bool removeReal(CoordinateNode* pNode);
		void removeDelNodes();
		void releaseNodes();

		/**
		��ĳ���ڵ��б䶯ʱ����Ҫ��������list�е�
		���λ�õ���Ϣ
		*/
		void update(CoordinateNode* pNode);

	protected:
		bool isEmpty();

		void moveNodeX(CoordinateNode* pNode, float px, CoordinateNode* pCurrNode);
		void moveNodeY(CoordinateNode* pNode, float py, CoordinateNode* pCurrNode);
		void moveNodeZ(CoordinateNode* pNode, float pz, CoordinateNode* pCurrNode);

	private:
		unsigned int size_;

		// �������βָ��
		CoordinateNode* first_x_coordinateNode_;
		CoordinateNode* first_y_coordinateNode_;
		CoordinateNode* first_z_coordinateNode_;

		std::list<CoordinateNode*> dels_;
		size_t dels_count_;

		std::list<CoordinateNode*> releases_;

		int updating_;
	};
}