#ifndef UTILITY_SINGLETON_H_
#define UTILITY_SINGLETON_H_


namespace util{
	template <typename T>
	struct SSingleton
	{
		static T* Instance()
		{
			static T instance;
			return &instance;
		}
	};
};


#endif /* UTILITY_SINGLETON_H_ */
