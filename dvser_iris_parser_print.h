#include "ypc/stbox/ebyte.h"
#include "ypc/stbox/stx_common.h"
#ifdef YPC_SGX
#include "ypc/stbox/tsgx/log.h"
#else
#include <glog/logging.h>
#endif
#include "user_type.h"

#include "ypc/corecommon/to_type.h"
#include <hpda/extractor/raw_data.h>
#include <hpda/output/memory_output.h>
#include <hpda/processor/processor_base.h>
#include <hpda/processor/transform/concat.h>

define_nt(iid, int);

typedef ff::util::ntobject<sepal_len, sepal_wid, petal_len, petal_wid, species>
    extra_nt_t;
class transform_format
    : public hpda::processor::processor_base_t<extra_nt_t, user_item_t> {
public:
  transform_format(::hpda::processor_with_output_t<extra_nt_t> *t)
      : hpda::processor::processor_base_t<extra_nt_t, user_item_t>(t) {}

  virtual bool process() {
    if (!has_input_value()) {
      return false;
    }

    auto t = base::input_value();
    m_data.get<iris_data>().set<sepal_len, sepal_wid, petal_len, petal_wid>(
        t.get<sepal_len>(), t.get<sepal_wid>(), t.get<petal_len>(),
        t.get<petal_wid>());
    m_data.set<species>(t.get<species>());
    base::consume_input_value();
    return true;
  }
  virtual user_item_t output_value() { return m_data; }

protected:
  user_item_t m_data;
};
class try_iris_parser {
public:
  try_iris_parser(ypc::data_source<stbox::bytes> *source)
      : m_source(source){};

  //inline stbox::bytes do_parse(const stbox::bytes &param) {
  inline stbox::bytes do_parse(const stbox::bytes &param) {

    ypc::to_type<stbox::bytes, extra_nt_t> converter(m_source);
    transform_format trans(&converter);
    hpda::output::internal::memory_output_impl<user_item_t> mo(&trans);
    mo.get_engine()->run();
    stbox::bytes result;
    int i = 0;
    for (auto it : mo.values()) {
      result += it.get<species>();
      //result += " - ";
      //auto id = it.get<kmeans_t::mean_point>();
      //result += " (" + std::to_string(id.template get<sepal_len>());
      //result += " ," + std::to_string(id.get<sepal_wid>());
      //result += " ," + std::to_string(id.get<petal_len>());
      //result += " ," + std::to_string(id.get<petal_wid>());
      //result += ") ";
     // result += "\n";
      i++;
    }
    //stbox::bytes result;
    return result;

      // LOG(INFO) << i << ": " << it.get<species>() << " - "
      //<< std::to_string(it.get<iid>());
    //return result;
  }
protected:
  ypc::data_source<stbox::bytes> *m_source;
};
