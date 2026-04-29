export type Device = {
    name: string
    variables: {
        name: string
    }[]
}

export type Kernel = {
    get_device(name: string): Device
}