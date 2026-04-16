#pragma once

#include <list>

#include "../utils.hpp"
#include "base.hpp"

namespace metaheuristics
{
    class TS : public Base
    {
    public:
        struct TS_Parameters
        {
            int diversify_after;
        };

    public:
        TS(Parameters parameters, TS_Parameters ts_parameters) : Base(parameters), ts_parameters_{ts_parameters} {}

    private:
        class Tabulist
        {
        public:
            inline void SetTenure(size_t tenure) { tenure_ = tenure; Refresh(); }
            
            inline bool IsTabu(const InstanceHandler::Edge& edge) { 
                return std::find(tabulist_.begin(), tabulist_.end(), edge) != tabulist_.end(); 
            }
            
            inline void AddTabu(const InstanceHandler::Edge& edge) { 
                tabulist_.push_back(edge); 
                Refresh(); 
            }
            
            inline void RemoveTabu(const InstanceHandler::Edge& edge) { tabulist_.remove(edge); }
            
            inline void Reset() { tabulist_.clear(); }

        private:
            size_t tenure_;
            std::list<InstanceHandler::Edge> tabulist_;

            inline void Refresh() { while (tabulist_.size() > tenure_) tabulist_.pop_front(); }
        };

    private:
        static constexpr std::string kAlgorithmName{"TS"};

        TS_Parameters ts_parameters_;
        Solution individual_{solution_};
        Tabulist tabulist_;
        int diversify_after_cnt_;

        inline void UpdateTenure() {
            size_t n = parameters_.instance_handler.GetInstance().GetNumNodes();
            size_t tenure_range_inf = n / 2;
            size_t tenure_range_sup = n;
            tabulist_.SetTenure(Random_A_B(tenure_range_inf, tenure_range_sup));
        }

    private:
        inline const std::string &GetAlgorithmName() const override { return kAlgorithmName; };

        void Prepare() override;
        void Iterate() override;

        void ReportHeaderAdditional() const override;
    };
} // metaheuristics
