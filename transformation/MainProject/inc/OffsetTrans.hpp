#pragma once

#include "Common.hpp"
#include "VectorialTrans.hpp"

namespace IMT {
class OffsetTrans: public VectorialTrans
{
    public:
        OffsetTrans(std::shared_ptr<LayoutView> baseLayout, float vectorOffsetRatio, Coord3dCart b): VectorialTrans(baseLayout), m_vectorOffsetRatio(vectorOffsetRatio), m_b(std::move(b)) {}
        ~OffsetTrans(void) = default;

    private:
        float m_vectorOffsetRatio;
        Coord3dCart m_b;

        virtual Coord3dCart FromBeforeTrans3dToAfterTrans3d(Coord3dCart vectBefore) const override
        {
            Coord3dCart v = vectBefore+m_vectorOffsetRatio*m_b;
            return v/v.Norm();
        }

        virtual Coord3dCart FromAfterTrans3dToBeforeTrans3d(Coord3dCart vectAfter) const override
        {
            SCALAR tmp = vectAfter.DotProduct(-m_vectorOffsetRatio*m_b);
            return (-tmp + std::sqrt(tmp*tmp-m_vectorOffsetRatio*m_vectorOffsetRatio+1))*vectAfter-m_vectorOffsetRatio*m_b;
        } 

};

class DecoratorConfigOffsetTrans: public DecoratorConfigParserBase
{
    public:
        DecoratorConfigOffsetTrans(std::string key): DecoratorConfigParserBase(key),
            m_offsetRatio(this, "offsetRatio", "Ratio of the offset transformation.", false),
            m_emphDirection(this, "emphDirection", "Vector indicating the emphasize direction. [Default = {\"type\":\"vectorCart\", \"x\": 1, \"y\":0, \"z\":0}]", true, "{\"type\":\"vectorCart\", \"x\": 1, \"y\":0, \"z\":0}")
         {}

        std::shared_ptr<LayoutView> Create(std::shared_ptr<LayoutView> baseLayout, std::string layoutSection, pt::ptree& ptree) const override
        {
            auto b = ParseVectorJSON(m_emphDirection.GetValue(layoutSection, ptree));
            return std::make_shared<OffsetTrans>(baseLayout, m_offsetRatio.GetValue(layoutSection, ptree), b);
        }
    private:
        KeyTypeDescription<SCALAR> m_offsetRatio;
        KeyTypeDescription<std::string> m_emphDirection;
};



}
