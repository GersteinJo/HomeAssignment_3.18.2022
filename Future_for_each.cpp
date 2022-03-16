#include <iostream>
#include <future>
#include <algorithm>
#include <vector>
#include <random>
#include <chrono>

template<typename Iterator, typename lambda>
void Future_for_each_block(Iterator begin, Iterator end, const lambda& action)
{
    std::for_each(begin, end, action);
}


template<typename Iterator, typename lambda>
void Future_for_each(Iterator begin, Iterator end, const lambda& action, int num_streams)
{
    int len =std::distance(begin,end);
    if(len<num_streams*4)
    {
        std::for_each(begin, end, action);
    }
    else
    {
        int len_per_stream = len / num_streams;
        std::vector<std::future<void>> working_streams;
        for(int i = 0; i< num_streams-1; i++)
        {
            working_streams.push_back(std::async(std::launch::async, Future_for_each_block<Iterator,lambda>,
                                                 std::next(begin, i*len_per_stream),
                                                 std::next(begin, (i+1)*len_per_stream), action));
        }
        working_streams.push_back(std::async(std::launch::async, Future_for_each_block<Iterator,lambda>,
                                             std::prev(end, len_per_stream+1),
                                             end, action));
        for(auto& stream : working_streams)
        {
            stream.get();
        }
    }
}

int main()
{
    //here we generate a sequence of 10 int elems and print it out to the console
    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    auto generator = std::mt19937(seed);
    auto distribution = std::uniform_int_distribution(1, 100);
    std::vector<int> myVector;
    std::generate_n(std::back_inserter(myVector),39,
                    [&generator, &distribution]()
                    {
                        return distribution(generator);
                    });
    std::cout<<"Initial sequence: ";
    std::for_each(std::begin(myVector), std::end(myVector), [](auto arg){std::cout<<arg<<" ";});
    std::cout<<"\n";

    std::cout<<"Sequence after multiplication by 3: ";
    //here we apply the function Future_for_each to the sequence
    auto my_lambda = [](int &n){ n *= 3; };
    Future_for_each(std::begin(myVector),std::end(myVector), my_lambda, 8);
    std::for_each(std::begin(myVector), std::end(myVector), [](auto arg){std::cout<<arg<<" ";});
    std::cout<<"\n";
}
