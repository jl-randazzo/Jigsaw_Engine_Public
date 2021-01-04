#ifndef _ASYNC_JOB_H_
#define _ASYNC_JOB_H_
namespace Jigsaw {
	namespace System {

		class ASYNC_EVENT {
		public:
			ASYNC_EVENT(LPCSTR str) : handle(CreateEvent(0, TRUE, FALSE, str)) {}

			void Notify() {
				SetEvent(handle);
			}

			void Await() const {
				WaitForSingleObject(handle, INFINITE);
			}

			bool Check() const {
				DWORD d_ = WaitForSingleObject(handle, 0);
				return d_ == WAIT_OBJECT_0;
			}

		protected:
			const HANDLE handle;

		};

		class EVENT_LISTENER {
		public:
			EVENT_LISTENER(const ASYNC_EVENT& _event) : _event(_event) {}

			void Await() {
				_event.Await();
			}

			bool Check() const {
				return _event.Check();
			}

		private:
			ASYNC_EVENT _event;

		};

		enum NOTIFY {
			ASYNC_NOTIFY,
			OWNER_NOTIFY
		};

		/// <summary>
		/// Simple base class only designed to locally encapsulate the parameters needed for a job. 
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template<class T>
		class ASYNC_JOB {

		public:
			ASYNC_JOB(NOTIFY notify = ASYNC_NOTIFY) : _event("LOAD_JOB_EVENT") {
				listener = Jigsaw::MakeRef<EVENT_LISTENER>(_event);
				if (notify == ASYNC_NOTIFY) {
					fut = std::async(std::launch::async, &ASYNC_JOB<T>::ExecuteNotify, this, &_event);
				}
				else { // OWNER_NOTIFY, the creator of this job takes responsibility for notifying listeners of its completion
					fut = std::async(std::launch::async, &ASYNC_JOB<T>::Execute, this);
				}
			}

			virtual inline bool Ready() {
				return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
			}

			virtual T Await() {
				fut.wait();
				return Get();
			}

			virtual T Get() {
				return fut.get();
			}

			Jigsaw::Ref<const EVENT_LISTENER> GetListener() const {
				return listener;
			}

			ASYNC_EVENT& GetEvent() {
				return _event;
			}

		protected:
			virtual T Execute() = 0;

			std::future<T> fut;

		private:
			T ExecuteNotify(ASYNC_EVENT* notify) {
				T ret = Execute();
				notify->Notify();
				return ret;
			}

			ASYNC_EVENT _event;
			Jigsaw::Ref<EVENT_LISTENER> listener;
		};
	}
}
#endif