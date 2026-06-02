export type Driver = {
    name: string
    variables: {
        name: string
    }[]
}

export type Kernel = {
    get_driver(name: string): Driver
}