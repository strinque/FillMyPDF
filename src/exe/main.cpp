#include <string>
#include <fstream>
#include <filesystem>
#include <functional>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/color.h>
#include <nlohmann/json.hpp>
#include <winpp/console.hpp>
#include <winpp/parser.hpp>
#include <winpp/utf8.hpp>
#include "FillMyPDF.h"
using json = nlohmann::json;

/*============================================
| Declaration
==============================================*/
// program version
const std::string PROGRAM_NAME = "FillMyPDF";
const std::string PROGRAM_VERSION = "1.3.0";

// default length in characters to align status 
constexpr std::size_t g_status_len = 50;

/*============================================
| Function definitions
==============================================*/
// lambda function to show colored tags
auto add_tag = [](const fmt::color color, const std::string& text) {
  fmt::print(fmt::fg(color) | fmt::emphasis::bold, "[{}]\n", text);
};

// execute a sequence of actions with tags
void exec(const std::string& str, std::function<void()> fct)
{
  fmt::print(fmt::emphasis::bold, "{:<" + std::to_string(g_status_len) + "}", str + ": ");
  try
  {
    fct();
    add_tag(fmt::color::green, "OK");
  }
  catch (const std::exception& ex)
  {
    add_tag(fmt::color::red, "KO");
    throw ex;
  }
}

// parse json data
void parse_json(const std::filesystem::path& filepath,
                std::map<std::string, std::string>& texts,
                std::map<std::string, bool>& checkboxes)
{
  // open file
  std::ifstream file(filepath);
  if (!file)
    throw std::runtime_error(fmt::format("can't read json-data file: {}", filepath.filename().u8string()));

  // parse json file
  json data = json::parse(file);

  // read text fields
  const auto& txt_obj = data["text"];
  for (auto& t : txt_obj)
    texts[t["name"].get<std::string>()] = utf8::from_utf8(t["value"].get<std::string>());

  // read checkbox fields
  const auto& checkbox_obj = data["checkbox"];
  for (auto& c : checkbox_obj)
    checkboxes[c["name"].get<std::string>()] = c["value"].get<bool>();
}

int main(int argc, char *argv[])
{
  // initialize console on Windows
  console::init();

  // parse command-line arguments
  std::filesystem::path pdf_input;
  std::filesystem::path pdf_output;
  std::filesystem::path json_file;
  console::parser parser(PROGRAM_NAME, PROGRAM_VERSION);
  parser.add("i", "pdf-input", "set the input pdf file to fill", pdf_input, true)
        .add("o", "pdf-output", "set the output filled pdf file", pdf_output, true)
        .add("j", "json-data", "set the json fields file", json_file, true);
  if (!parser.parse(argc, argv))
  {
    parser.print_usage();
    return -1;
  }

  try
  {
    // check arguments validity
    if (!std::filesystem::exists(pdf_input))
      throw std::runtime_error(fmt::format("pdf-input file: \"{}\" doesn't exists", pdf_input.u8string()));
    if (!std::filesystem::exists(json_file))
      throw std::runtime_error(fmt::format("json-data file: \"{}\" doesn't exists", json_file.u8string()));

    // start the process
    fmt::print(fmt::fg(fmt::color::light_blue) | fmt::emphasis::bold, "execute the filling process of pdf:\n");

    // open and extract pdf input fields using PoDoFo library
    FillMyPDF pdf;
    exec("open and parse pdf", [&]() { pdf.load(pdf_input); });

    // read and parse json file using nlohmann json library
    std::map<std::string, std::string> texts;
    std::map<std::string, bool> checkboxes;
    exec("parse json data", [&]() { parse_json(json_file, texts, checkboxes); });

    // fill pdf file with data
    exec("fill pdf file", [&]() { pdf.fill(texts, checkboxes); });

    // save pdf file with new data
    exec("save pdf file", [&]() { pdf.save(pdf_output); });
    return 0;
  }
  catch (const std::exception& ex)
  {
    fmt::print(fmt::format("{} {}\n",
      fmt::format(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error:"),
      ex.what()));
    return -1;
  }
}