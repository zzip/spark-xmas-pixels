require 'rubygems'
require 'ruby_spark'
require 'ap'
require 'yaml'

env = 'default'
CONFIG = YAML.load_file("./config.yml")[env]

RubySpark.configuration do |config|
  config.access_token = CONFIG['auth_token']
  config.timeout      = 10 #.seconds # defaults to 30 seconds
end

# TODO: loop through your different cores
$core = RubySpark::Core.new(CONFIG['devices'][0])
ap $core.info

def send_to_core(cmd: "handleparams", argument: "cmd:w,r:255,g:255,b:255,wait:100")
  #puts "sending #{cmd} #{argument}"

  start = Time.now
  # TODO: loop through your different cores
  result = $core.function cmd,argument
  stop = Time.now

  puts "call #{cmd} #{argument} took #{stop-start}s result #{result}"
  return "call #{cmd} #{argument} took #{stop-start}s result #{result}"
end
