#include <LayoutFactory.hpp>
#include <Layout.hpp>

#include <exception>

using namespace IMT;

LayoutFactory& LayoutFactory::Instance()
{
    static LayoutFactory factory;
    return factory;
}

void LayoutFactory::RegisterMaker(const std::string& key, LayoutConfigParserBase* maker)
{
    if (_makers.find(key) != _makers.end())
    {
        throw std::invalid_argument("Multiple makers for the same key: "+ key);
    }
    _makers[key] = maker;
}

std::string LayoutConfigParserBase::GetHelp(void) const
{
    std::string out ("type = "+m_layoutConfigParserId+"\n");
    for (auto ktd: m_keyTypeDescriptionMap)
    {
        out += "\t\e[1m"+std::get<0>(ktd) + "\e[0m: " + std::string(std::get<1>(ktd)->IsOptional() ? "[\e[4mOptional\e[0m]" : "") + std::get<1>(ktd)->GetDescription()+"\n";
    }
    return std::move(out);
}

void LayoutConfigParserBase::AddKeyTypeDescription(KeyTypeDescriptionBase* ktd)
{
    if (m_keyTypeDescriptionMap.find(ktd->GetLabel()) != m_keyTypeDescriptionMap.end())
    {
        throw std::invalid_argument("Multiple arguments with the same key: "+ktd->GetLabel());
    }
    m_keyTypeDescriptionMap[ktd->GetLabel()] = ktd;
}

std::string LayoutFactory::GetHelp(void) const
{
    std::string out;
    for (auto& kv: _makers)
    {
        out += "#######\n";
        out += std::get<1>(kv)->GetHelp() + "\n";
        out += "*******\n";
    }
    return out;
}

std::string LayoutFactory::GetHelp(std::string key) const
{
    std::string out;
    if (_makers.find(key) != _makers.end())
    {
        out += _makers.at(key)->GetHelp() + "\n";
    }
    else
    {
        out += key + " is not a known layout type";
    }
    return out;
}

Quaternion IMT::ParseRotationJSON(std::string s)
{
  std::stringstream ss(s);
  pt::ptree ptree_json;

  pt::json_parser::read_json(ss, ptree_json);

  auto& v = ptree_json.get_child("type");

  if (v.data() == "euler")
  {
    auto vv = ptree_json.get_child("yaw");
    auto y = std::stod(vv.data())*PI()/180;

    vv = ptree_json.get_child("pitch");
    auto p =  std::stod(vv.data())*PI()/180;

    vv = ptree_json.get_child("roll");
    auto r = std::stod(vv.data())*PI()/180;
    return Quaternion::FromEuler(y, p, r);
  }
  else if (v.data() == "quaternion")
  {
    auto vv = ptree_json.get_child("w");
    auto w = std::stod(vv.data());

    vv = ptree_json.get_child("x");
    auto x =  std::stod(vv.data());

    vv = ptree_json.get_child("y");
    auto y = std::stod(vv.data());

    vv = ptree_json.get_child("z");
    auto z = std::stod(vv.data());
    auto q = Quaternion(w, VectorCartesian(x, y, z));
    q.Normalize();
    return q;
  }
  else if (v.data() == "angleAxis")
  {
    auto vv = ptree_json.get_child("angle");
    auto theta = std::stod(vv.data())*PI()/180;

    vv = ptree_json.get_child("x");
    auto x =  std::stod(vv.data());

    vv = ptree_json.get_child("y");
    auto y = std::stod(vv.data());

    vv = ptree_json.get_child("z");
    auto z = std::stod(vv.data());

    VectorCartesian v(x, y, z);
    auto q = Quaternion(std::cos(theta/2.0), std::cos(theta/2.0)*v/v.Norm());
    q.Normalize();
    return q;
  }

  // BOOST_FOREACH(boost::property_tree::ptree::value_type &v, ptree_json.get_child("particles.electron"))
  // {
  //     assert(v.first.empty()); // array elements have no names
  //     std::cout << v.second.data() << std::endl;
  //     // etc
  // }
  return Quaternion(0);
}

Coord3dCart IMT::ParseVectorJSON(std::string s)
{
  std::stringstream ss(s);
  pt::ptree ptree_json;

  pt::json_parser::read_json(ss, ptree_json);

  auto& v = ptree_json.get_child("type");

  if (v.data() == "vectorCart")
  {
    auto vv = ptree_json.get_child("x");
    auto x = std::stod(vv.data());

    vv = ptree_json.get_child("y");
    auto y =  std::stod(vv.data());

    vv = ptree_json.get_child("z");
    auto z = std::stod(vv.data());
    return Coord3dCart(x, y, z);
  }
  else if (v.data() == "vectorSpherical")
  {
    auto vv = ptree_json.get_child("rho");
    auto rho = std::stod(vv.data());

    vv = ptree_json.get_child("theta");
    auto theta =  std::stod(vv.data())*PI()/180.f;

    vv = ptree_json.get_child("phi");
    auto phi = std::stod(vv.data())*PI()/180.f;
    return Coord3dSpherical(rho, theta, phi);
  }
  else if (v.data() == "rotation")
  {
    auto vv = ptree_json.get_child("rotation");
    std::ostringstream outSs;
    pt::json_parser::write_json(outSs, vv);
    auto q = ParseRotationJSON(outSs.str());
    return q.Rotation(Coord3dCart(1, 0, 0));
  }
  return Coord3dCart(0, 0, 0);
}

std::shared_ptr<VectorialTrans> IMT::GetVectorialTransformation(std::string transSection, pt::ptree& ptree, const Quaternion& layoutRotation)
{
    if (transSection.empty())
    {
        return std::make_shared<VectorialTrans>();
    }
    std::string transType;
    try
    {
        transType = ptree.get<std::string>(transSection+".vectorSpaceTransformationType");
        if (transType == "offsetTrans")
        {
            auto offsetRatio = ptree.get<double>(transSection+".offsetRatio");
            auto vector_b_conf = ptree.get_optional<std::string>(transSection+".emphDirection");
            Coord3dCart b(0, 0, 0);
            if (vector_b_conf)
            {
                b = ParseVectorJSON(vector_b_conf.get());
                b = b/b.Norm();
            }
            else
            {
                b = layoutRotation.Rotation(Coord3dCart(1, 0, 0));
            }
            std::cout << "offsetTrans -> alpha = " << offsetRatio << "; b = " << b << std::endl;
            return std::make_shared<OffsetTrans>(offsetRatio, std::move(b));
        }
        if (transType == "horizontalOffsetTrans")
        {
            auto offsetRatio = ptree.get<double>(transSection+".offsetRatio");
            auto orientation_conf = ptree.get_optional<std::string>(transSection+".orientation");
            Quaternion q;
            if (orientation_conf)
            {
                q = ParseRotationJSON(orientation_conf.get());
            }
            else
            {
                q = layoutRotation;
            }
            std::cout << "horizontalOffsetTrans -> alpha = " << offsetRatio << "; q = " << q << std::endl;
            return std::make_shared<HorizontalOffsetTrans>(offsetRatio, std::move(q));
        }

    }
    catch (std::exception &e)                                                    
    {                                                                            
        std::cout << "Error while parsing in configuration file the "<<transSection<<" Vectorial Transformation: " << e.what() << std::endl;
        throw e;                                                                 
    }                                                                            
    throw std::invalid_argument("Not supported type: "+transType);
}
