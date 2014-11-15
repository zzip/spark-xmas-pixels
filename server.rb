require 'rubygems'
require 'bundler'
require 'sinatra'
require './api_test.rb'

configure do
  set :bind, "0.0.0.0"
end

get '/' do
  erb :index
end

get '/generic/:function/:status' do
  @result = send_to_core(cmd: params[:function], argument: params[:status])

  erb :index
end

post '/generic' do
  puts params.inspect
  args = []
  args << "E:#{params[:cmd]}" unless params[:cmd].to_s.empty?
  args << "c:#{params[:color]}" unless params[:color].to_s.empty?
  args << "w:#{params[:delay]}" unless params[:delay].to_s.empty?
  args << "C:#{params[:clear]}" unless params[:clear].to_s.empty?
  args << "B:#{params[:brightness]}" unless params[:brightness].to_s.empty?

  @result = send_to_core(cmd: "handleparams", argument: args.join(',') )

  erb :index
end

post '/sequence' do
  puts params.inspect

  @result = send_to_core(cmd: "setsequence", argument: params[:sequence] )

  erb :index

end

get '/debug/:cmd/:color/:delay' do
  puts params.inspect
  args = []
  args << "E:#{params[:cmd]}" unless params[:cmd].to_s.empty?
  args << "c:#{params[:color]}" unless params[:color].to_s.empty?
  args << "w:#{params[:delay]}" unless params[:delay].to_s.empty?
  args << "C:#{params[:clear]}" unless params[:clear].to_s.empty?
  args << "B:#{params[:brightness]}" unless params[:brightness].to_s.empty?

  @result = send_to_core(cmd: "handleparams", argument: args.join(',') )

  erb :index
end

get '/wipe_rgb/:red/:green/:blue' do
  @result = send_to_core(cmd: "handleparams", argument: "E:w,r:#{params[:red]},g:#{params[:green]},b:#{params[:blue]}")

  erb :index
end

get '/wipe_hex/:color' do
  @result = send_to_core(cmd: "handleparams", argument: "E:w,c:#{params[:color]}")

  erb :index
end

post '/adjust_brightness' do
  @result = send_to_core(cmd: "handleparams", argument: "B:#{params[:brightness]}")

  erb :index
end

#CI Project
=begin
get '/setcolor/:hexvalues' do
  @result = send_to_core(cmd: "buildstatus", argument: "##{params[:hexvalues]}")

  erb :index
end

get '/rgb/:red/:green/:blue' do
  @result = send_to_core(cmd: "buildstatus", argument: "##{params[:red]}#{params[:green]}#{params[:blue]}")

  erb :index
end
=end
