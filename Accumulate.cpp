/* при распараллеливании программы целесообразно ответить на 3 вопроса:
 * Нужно ли распараллеливание для решения задачи?
 * Возможно ли распарарллеливание программы?
 * Стоит ли оно того, т.е., нет ли возможности решить задачу проще без потери производительности
 */

/*
 *                                       Распараллеливание
 * Процессы                                         Потоки
 * количество ядер == количество процессов          Работают как процессы, но могут делиться
 * ввиду того, что ядер обычно меньше, чем          памятью:
 * процессов существует понятие переключение        +: быстрее процессов, менее затратны
 * контекста: процесс может выполняться не          -: менее безопасны, чем процессы
 * более определенного количества, после чего
 * его состояние сохраняется, а процесс
 * встает в очередь на обработку
 *
 *
 *
 */


#include <thread>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <vector>
#include <chrono>


template <typename Iterator, typename T>
void accumulate_block(Iterator first, Iterator last, T init, T& result) {
    result = std::accumulate(first, last, init);
}

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init, unsigned int num_thread) {
    // 1. Проверили длину
    auto length = std::distance(first, last);
    if (length < 32) {
        return std::accumulate(first, last, init);
    }
    // 2. Длина достаточна, распараллеливаем
    auto num_workers = num_thread;
    // Вычислили длину для одного потока
    auto length_per_thread = (length) / num_workers;
    // Векторы с потоками и результатами
    std::vector<std::thread> threads;
    std::vector<T> results(num_workers - 1);
    // 3. Распределяем данные (концепция полуинтервалов!)
    for(auto i = 0u; i < num_workers - 1; i++) {
        auto beginning = std::next(first, i * length_per_thread);
        auto ending = std::next(first, (i + 1) * length_per_thread);
        // 4. Запускаем исполнителей
        threads.push_back(std::thread(
                accumulate_block<Iterator, T>,
                beginning, ending, 0, std::ref(results[i])));
    }
    // Остаток данных -- в родительском потоке
    auto main_result = std::accumulate(std::next(first, (num_workers - 1) * length_per_thread),
                                       last, init);
    // std::mem_fun_ref -- для оборачивания join().
    std::for_each(std::begin(threads), std::end(threads), std::mem_fun_ref(&std::thread::join));
    // 5. Собираем результаты
    return std::accumulate(std::begin(results), std::end(results), main_result);
}

template<typename Unit>
class Timer
{
private:
    std::chrono::steady_clock::time_point start;
public:
    Timer()=default;
    void Start()
    {
        start = std::chrono::steady_clock::now();
    }
    int Stop()
    {
        return std::chrono::duration_cast<Unit>(std::chrono::steady_clock::now()-start).count();
    }

};

int main() {
    std::vector<int> test_sequence(100u);
    std::iota(test_sequence.begin(), test_sequence.end(), 0);


    int N = 19; //num of streams
    Timer<std::chrono::microseconds> myTimer;
    std::vector<int> Plot_num_time;
    for(int i = 1; i<N+1; i++)
    {
        myTimer.Start();
        auto result = parallel_accumulate(std::begin(test_sequence), std::end(test_sequence), 0, i);
        int time_i = myTimer.Stop();
        Plot_num_time.push_back(time_i);
        std::cout<<result<<"\n";
    }
    for(int i = 0; i < N; i++) std::cout<<i+1<<" "<<Plot_num_time[i]<<"\n";
}
