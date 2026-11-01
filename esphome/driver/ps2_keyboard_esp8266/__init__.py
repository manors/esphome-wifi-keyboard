import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

ps2_ns = cg.global_ns.namespace("ps2")
PS2KeyboardComponent = ps2_ns.class_("PS2KeyboardComponent", cg.Component)

CONF_CLK_PIN = "clk_pin"
CONF_DATA_PIN = "data_pin"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PS2KeyboardComponent),
    cv.Required(CONF_CLK_PIN): cv.int_,
    cv.Required(CONF_DATA_PIN): cv.int_,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_CLK_PIN], config[CONF_DATA_PIN])
    await cg.register_component(var, config)