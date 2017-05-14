#pragma once

class noncopyable
{
protected:
	noncopyable( noncopyable&& ) noexcept =default;
	noncopyable& operator=(noncopyable&& ) =default;


	noncopyable() {}
	~noncopyable() {}
private:
	noncopyable( const noncopyable& );
	const noncopyable& operator=( const noncopyable& );
};

