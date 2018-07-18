#pragma once


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <memory>
#include <map>
#include <string>

#include "Quaternion.hpp"
#include "Common.hpp"

#include "OffsetTrans.hpp"
#include "HorizontalOffsetTrans.hpp"

namespace IMT {

namespace pt = boost::property_tree;

//Forward declaration
class Layout;

class KeyTypeDescriptionBase
{
    public:
        const std::string& GetLabel(void) const {return m_label;}
        const std::string& GetDescription(void) const {return m_description;}
        const bool& IsOptional(void) const {return m_optional;}

    protected:
        KeyTypeDescriptionBase(std::string label, std::string description, bool optional):
            m_label(std::move(label)), m_description(std::move(description)), m_optional(optional)
        {}

    private:
        std::string m_label;
        std::string m_description;
        bool m_optional;
};

class LayoutConfigParserBase;

class LayoutFactory
{//singleton class used to build concret layouts
    public:
        //get the singleton instance
        static LayoutFactory& Instance(void);

        //create a new layout by parsing the configuration file
        std::shared_ptr<Layout> Create(std::string layoutSection, pt::ptree& ptree) const; 
        //register a new layout maker
        void RegisterMaker(const std::string& key, LayoutConfigParserBase* maker);
        std::string GetHelp(void) const;
        std::string GetHelp(std::string key) const;
    private:
        std::map<std::string, LayoutConfigParserBase*> _makers;
};

//Layout maker base abstract class
class LayoutConfigParserBase
{
    public:
        virtual std::shared_ptr<Layout> Create(std::string layoutSection, pt::ptree& ptree) const = 0;
        std::string GetHelp(void) const;
        virtual ~LayoutConfigParserBase(void) = default;

        void AddKeyTypeDescription(KeyTypeDescriptionBase* ktd);
    protected:
        LayoutConfigParserBase(const std::string& key): m_keyTypeDescriptionMap(), m_layoutConfigParserId(key)
        {

            LayoutFactory::Instance().RegisterMaker(key, this);
        }
    private:
        std::map<std::string, KeyTypeDescriptionBase*> m_keyTypeDescriptionMap;
        std::string m_layoutConfigParserId;
};

//Macro to register a new layout config parser (require the parser to be defined first)
#define REGISTER_LAYOUT(key, LayoutConfigParser) static IMT::LayoutConfigParser maker(key);

template<typename T>
class KeyTypeDescription: public KeyTypeDescriptionBase
{
    public:
        KeyTypeDescription(LayoutConfigParserBase* lcpb, std::string label, std::string description, bool optional, T defVal = T()):
            KeyTypeDescriptionBase(label, description, optional),
            m_defVal(std::move(defVal))
        { lcpb->AddKeyTypeDescription(this); }


        T GetValue(const std::string& sectionContext, pt::ptree& ptree) const
        {
            if (IsOptional())
            {
                auto v = ptree.get_optional<T>(sectionContext+"."+GetLabel());
                if (v)
                {
                    return v.get();
                }
                else
                {
                    return std::move(m_defVal);
                }
            }
            else
            {
                return ptree.get<T>(sectionContext+"."+GetLabel());
            }
        }
    private:
        T m_defVal;
};

Quaternion ParseRotationJSON(std::string s);

Coord3dCart ParseVectorJSON(std::string s);

std::shared_ptr<VectorialTrans> GetVectorialTransformation(std::string transSection, pt::ptree& ptree, const Quaternion& layoutRotation);

class KeyRotationDescription: public KeyTypeDescription<std::string>
{
    public:
        KeyRotationDescription(LayoutConfigParserBase* lcpb, std::string label, std::string description, bool optional, Quaternion defVal = Quaternion(1)):
            KeyTypeDescription<std::string>(lcpb, label,
                    "(json) "+description+" format: \e[2mif type is \"euler\" you have to give the yaw, pitch and roll angle in degree {\"type\":\"euler\", \"yaw\":0.0, \"pitch\":0.0, \"roll\":0.0} ; else if type is \"quaternion\" you have to give the w, x, y, z value of the quaternion that represente the rotation. A normalization is performed by the software. {\"type\":\"quaternion\", \"w\":1.0, \"x\":0.0, \"y\":0.0, \"z\":0.0} ; else if type is \"angleAxis\" you have to give an \"angle\" in degree and a direction vector x, y, z that generates the rotation axis (this vector should not be (0, 0, 0)). {\"type\":\"angleAxis\", \"angle\":90, \"x\":0, \"y\":0, \"z\":1}\e[0m",
                    optional, "")
        {}

        Quaternion GetRotation(const std::string& sectionContext, pt::ptree& ptree) const
        {
            auto s = GetValue(sectionContext, ptree);
            if (s.size() == 0)
            {
                return m_defVal;
            }
            else
            {
                return ParseRotationJSON(s);
            }
        }
    private:
        Quaternion m_defVal;
};


}
