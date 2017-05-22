/******************************************
µ¥¼þ¶ÔÏñ
*******************************************/

#pragma  once


template <typename T> class Singleton
{
protected:

	static T* s_pInstance;

public:
	Singleton( void )
	{
		assert( !s_pInstance );
		s_pInstance = static_cast< T* >( this );
	}

	virtual  ~Singleton( void )
	{
		assert( s_pInstance );  
		s_pInstance = 0;  
	}

	static T& GetSingleton( void )
	{
		assert( s_pInstance );  
		return ( *s_pInstance ); 
	}


	static T* GetSingletonPtr( void )
	{ 
		return s_pInstance; 
	}

};
